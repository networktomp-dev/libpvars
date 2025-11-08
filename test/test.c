#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pvars.h"
#include "plist.h"
#include "pdict.h"
#include "pvars_internal.h" // For pvar_destroy_internal prototype

// Global pvars_errno declaration (to link with the one defined in perrno.c)
extern __thread int pvars_errno;

#define TEST_PASS() (printf("  \033[32m[PASS]\033[0m %s\n", __func__))
#define TEST_FAIL(msg) (printf("  \033[31m[FAIL]\033[0m %s: %s\n", __func__, msg))

// --- Helper Functions for Test Setup ---

/**
 * @brief Creates a pvar_t struct holding a dynamically allocated string.
 */
pvar_t create_string_pvar(const char *str) {
    pvar_t pvar = { .type = PVAR_TYPE_STRING };
    pvar.data.s = strdup(str);
    assert(pvar.data.s != NULL);
    return pvar;
}

/**
 * @brief Creates a pvar_t struct holding a new plist_t.
 */
pvar_t create_list_pvar(void) {
    pvar_t pvar = { .type = PVAR_TYPE_LIST };
    // Create a list with an element to ensure plist_destroy has work to do
    pvar.data.ls = plist_create(5);
    assert(pvar.data.ls != NULL);
    plist_add_int(pvar.data.ls, 10); 
    return pvar;
}

/**
 * @brief Creates a pvar_t struct holding a new pdict_t.
 */
pvar_t create_dict_pvar(void) {
    pvar_t pvar = { .type = PVAR_TYPE_DICT };
    // Create a dict with an element to ensure pdict_destroy has work to do
    pvar.data.dt = pdict_create(5);
    assert(pvar.data.dt != NULL);
    pdict_add_int(pvar.data.dt, "test_key", 20); 
    return pvar;
}

// --- Test Cases ---

/**
 * @brief Tests pvar_destroy_internal on a PVAR_TYPE_STRING.
 * Checks for type reset and NULLing of the pointer.
 */
void test_pvar_destroy_internal_string(void) {
    printf("Running %s...\n", __func__);
    pvar_t pvar = create_string_pvar("String to be freed");
    
    // Initial check
    if (pvar.type != PVAR_TYPE_STRING || pvar.data.s == NULL) {
        TEST_FAIL("Initial state incorrect");
        return;
    }

    pvar_destroy_internal(&pvar);
    
    // Verification: pointer must be NULLed and type must be NONE
    if (pvar.type != PVAR_TYPE_NONE) {
        TEST_FAIL("Type was not set to PVAR_TYPE_NONE");
        return;
    }
    if (pvar.data.s != NULL) {
        TEST_FAIL("String pointer was not set to NULL");
        return;
    }
    
    TEST_PASS();
}

/**
 * @brief Tests pvar_destroy_internal on a PVAR_TYPE_LIST.
 * Checks for recursive destruction (via plist_destroy) and pointer NULLing.
 */
void test_pvar_destroy_internal_list(void) {
    printf("Running %s...\n", __func__);
    pvar_t pvar = create_list_pvar();
    
    // Initial check
    if (pvar.type != PVAR_TYPE_LIST || pvar.data.ls == NULL) {
        TEST_FAIL("Initial state incorrect");
        return;
    }

    pvar_destroy_internal(&pvar);
    
    // Verification: pointer must be NULLed and type must be NONE
    if (pvar.type != PVAR_TYPE_NONE) {
        TEST_FAIL("Type was not set to PVAR_TYPE_NONE");
        return;
    }
    if (pvar.data.ls != NULL) {
        TEST_FAIL("List pointer was not set to NULL");
        return;
    }

    TEST_PASS();
}

/**
 * @brief Tests pvar_destroy_internal on a PVAR_TYPE_DICT.
 * Checks for recursive destruction (via pdict_destroy) and pointer NULLing.
 */
void test_pvar_destroy_internal_dict(void) {
    printf("Running %s...\n", __func__);
    pvar_t pvar = create_dict_pvar();
    
    // Initial check
    if (pvar.type != PVAR_TYPE_DICT || pvar.data.dt == NULL) {
        TEST_FAIL("Initial state incorrect");
        return;
    }

    pvar_destroy_internal(&pvar);
    
    // Verification: pointer must be NULLed and type must be NONE
    if (pvar.type != PVAR_TYPE_NONE) {
        TEST_FAIL("Type was not set to PVAR_TYPE_NONE");
        return;
    }
    if (pvar.data.dt != NULL) {
        TEST_FAIL("Dict pointer was not set to NULL");
        return;
    }

    TEST_PASS();
}

/**
 * @brief Tests pvar_destroy_internal on non-dynamic types and NULL input.
 */
void test_pvar_destroy_internal_edge_cases(void) {
    printf("Running %s...\n", __func__);
    
    // Case 1: PVAR_TYPE_INT (Non-dynamic, should only reset type)
    pvar_t pvar_int = { .type = PVAR_TYPE_INT, .data.i = 42 };
    int initial_int_value = pvar_int.data.i;
    pvar_destroy_internal(&pvar_int);
    if (pvar_int.type != PVAR_TYPE_NONE) {
        TEST_FAIL("INT: Type was not set to PVAR_TYPE_NONE");
        return;
    }
    // The data union should still hold the value (though not accessed via .i after reset)
    if (pvar_int.data.i != initial_int_value) { 
        TEST_FAIL("INT: Data unexpectedly modified");
        return;
    }

    // Case 2: PVAR_TYPE_NONE (Should do nothing and not crash)
    pvar_t pvar_none = { .type = PVAR_TYPE_NONE, .data.i = 0 };
    pvar_destroy_internal(&pvar_none);
    if (pvar_none.type != PVAR_TYPE_NONE) {
        TEST_FAIL("NONE: Type unexpectedly changed");
        return;
    }
    
    // Case 3: NULL Input (Should not crash, handled by initial guard)
    pvar_destroy_internal(NULL);
    
    TEST_PASS();
}


int main(void) {
    printf("--- pvar_destroy_internal Test Suite ---\n");
    
    test_pvar_destroy_internal_string();
    test_pvar_destroy_internal_list();
    test_pvar_destroy_internal_dict();
    test_pvar_destroy_internal_edge_cases();
    
    printf("--- Test Suite Complete ---\n");
    
    return 0;
}
