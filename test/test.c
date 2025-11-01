#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<math.h>     // For fabs() and fabsf()
#include<float.h>    // For FLT_EPSILON and DBL_EPSILON#include"pvars.h"

/*
 * NOTE: For this test to compile, you would typically need to
 * move 'plist_internal.h' contents into a new 'pvars_test_internal.h'
 * or similar, or simply include 'plist_internal.h' here.
 * Since 'pvar_t' is needed for plist_contains, we must include it.
 */
#include "plist_internal.h" // For pvar_t, pvar_equals (needed for plist_contains test)
#include "pvars.h"          // Public API
#include "perrno.h"         // Error handling

// Macro for simple pass/fail assertions with error reporting
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __func__, __LINE__, message); \
            fprintf(stderr, "      Errno: %d (%s)\n", pvars_errno, perror_message()); \
            return false; \
        } \
    } while (0)

// Helper to create a pvar_t for testing plist_contains
pvar_t create_test_pvar_str(const char *str) {
    pvar_t var;
    var.type = PVAR_TYPE_STRING;
    // We don't strdup here, as plist_contains will use pvar_equals on the pointer
    var.data.s = (char *)str;
    return var;
}

pvar_t create_test_pvar_int(int val) {
    pvar_t var;
    var.type = PVAR_TYPE_INT;
    var.data.i = val;
    return var;
}

// Global count for tracking tests run/passed
static int tests_run = 0;
static int tests_passed = 0;

/* --- Test Functions --- */

/**
 * @brief Tests plist_create, plist_destroy, and capacity logic.
 */
bool test_plist_creation_and_destruction() {
    tests_run++;
    plist_t *list = NULL;
    size_t initial_cap = 10;

    // Test 1: Invalid capacity
    list = plist_create(0);
    TEST_ASSERT(list == NULL, "plist_create(0) should fail");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_CREATE_CAPACITY_OUT_OF_BOUNDS, "Incorrect error code for invalid capacity");

    // Test 2: Successful creation
    list = plist_create(initial_cap);
    TEST_ASSERT(list != NULL, "plist_create(10) failed");
    TEST_ASSERT(plist_get_size(list) == 0, "New list size should be 0");
    TEST_ASSERT(plist_get_capacity(list) == initial_cap, "New list capacity incorrect");

    // Test 3: Destruction (should not crash)
    plist_destroy(list);
    list = NULL;

    tests_passed++;
    return true;
}

/**
 * @brief Tests all plist_add_* functions and the resizing logic.
 */
bool test_plist_add_and_resize() {
    tests_run++;
    plist_t *list = plist_create(2);
    TEST_ASSERT(list != NULL, "List creation failed for add test");

    // Add elements until resize is triggered
    plist_add_int(list, 100);
    TEST_ASSERT(plist_get_size(list) == 1, "Size incorrect after adding int");

    plist_add_str(list, "hello");
    TEST_ASSERT(plist_get_size(list) == 2, "Size incorrect after adding string");
    TEST_ASSERT(plist_get_capacity(list) == 2, "Capacity should still be 2");

    // Add one more to force resize (capacity should double to 4)
    plist_add_double(list, 3.14);
    TEST_ASSERT(plist_get_size(list) == 3, "Size incorrect after resize add");
    TEST_ASSERT(plist_get_capacity(list) == 4, "Capacity did not double after resize");

    plist_add_long(list, 99999999L);
    plist_add_float(list, 5.5f);
    TEST_ASSERT(plist_get_size(list) == 5, "Size incorrect after multiple adds");
    TEST_ASSERT(plist_get_capacity(list) == 8, "Capacity did not double correctly to 8");

    // Test plist_add_str NULL input
    plist_add_str(list, NULL);
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_ADD_NULL_STRING_INPUT, "plist_add_str(NULL) failed to set correct error");
    TEST_ASSERT(plist_get_size(list) == 5, "List size changed after failed add");

    // Test adding to NULL list
    plist_add_int(NULL, 1);
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_ADD_INT_NULL_INPUT, "plist_add_int(NULL) failed to set correct error");

    plist_destroy(list);
    tests_passed++;
    return true;
}

/**
 * @brief Tests all plist_get_* functions for correct value retrieval and type checking.
 */
bool test_plist_get_and_type_safety() {
    tests_run++;
    plist_t *list = plist_create(5);
    int int_out = 0;
    char *str_out = NULL;
    double dbl_out = 0.0;
    long long_out = 0L;
    float float_out = 0.0f;
    
    plist_add_int(list, 123);         // Index 0
    plist_add_str(list, "Testing");   // Index 1
    plist_add_double(list, 2.71828);  // Index 2
    plist_add_long(list, 10000000000L); // Index 3
    plist_add_float(list, 42.1f);     // Index 4

    // Test 1: Successful Int retrieval
    TEST_ASSERT(plist_get_int(list, 0, &int_out) == true, "plist_get_int failed");
    TEST_ASSERT(int_out == 123, "Incorrect int value retrieved");

    // Test 2: Successful String retrieval
    TEST_ASSERT(plist_get_str(list, 1, &str_out) == true, "plist_get_str failed");
    TEST_ASSERT(strcmp(str_out, "Testing") == 0, "Incorrect string value retrieved");

    // Test 3: Successful Double retrieval
    TEST_ASSERT(plist_get_double(list, 2, &dbl_out) == true, "plist_get_double failed");
    TEST_ASSERT(dbl_out == 2.71828, "Incorrect double value retrieved");
    
    // Test 4: Successful Long retrieval
    TEST_ASSERT(plist_get_long(list, 3, &long_out) == true, "plist_get_long failed");
    TEST_ASSERT(long_out == 10000000000L, "Incorrect long value retrieved");

    // Test 5: Successful Float retrieval
    TEST_ASSERT(plist_get_float(list, 4, &float_out) == true, "plist_get_float failed");
    // Use an epsilon check for float comparison
    TEST_ASSERT(fabsf(float_out - 42.1f) < 0.0001f, "Incorrect float value retrieved");

    // Test 6: Wrong Type check (Index 1 is string, try to get int)
    TEST_ASSERT(plist_get_int(list, 1, &int_out) == false, "plist_get_int on string should fail");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_GET_INT_WRONG_TYPE, "Incorrect error code for wrong type");

    // Test 7: Out of Bounds check
    TEST_ASSERT(plist_get_str(list, 5, &str_out) == false, "plist_get_str out of bounds should fail");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_GET_STR_OUT_OF_BOUNDS, "Incorrect error code for out of bounds");

    // Test 8: NULL list input
    TEST_ASSERT(plist_get_int(NULL, 0, &int_out) == false, "plist_get_int(NULL) should fail");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_GET_INT_NULL_INPUT, "Incorrect error code for NULL list");
    
    // Test 9: NULL output pointer for non-string get
    TEST_ASSERT(plist_get_double(list, 2, NULL) == false, "plist_get_double(NULL out_value) should fail");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_GET_DOUBLE_NULL_INPUT, "Incorrect error code for NULL out_value");


    plist_destroy(list);
    tests_passed++;
    return true;
}

/**
 * @brief Tests all plist_set_* functions, ensuring old data is freed and type changes correctly.
 */
bool test_plist_set_functions() {
    tests_run++;
    plist_t *list = plist_create(3);
    char *str_out = NULL;
    int int_out = 0;

    // Initial fill (Element 0 is a string that must be freed by plist_set_int)
    plist_add_str(list, "original_string");
    plist_add_int(list, 50);
    plist_add_double(list, 1.0);

    // Test 1: Change String to Int (Index 0)
    plist_set_int(list, 0, 99);
    TEST_ASSERT(pvars_errno == SUCCESS, "plist_set_int failed");
    TEST_ASSERT(plist_get_int(list, 0, &int_out) == true, "Failed to retrieve new int value");
    TEST_ASSERT(int_out == 99, "New int value incorrect");
    // Ensure previous string type is gone
    TEST_ASSERT(plist_get_str(list, 0, &str_out) == false, "Old string type still accessible");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_GET_STR_WRONG_TYPE, "Incorrect error code after type change");

    // Test 2: Change Int to String (Index 1)
    plist_set_str(list, 1, "new_string");
    TEST_ASSERT(plist_get_str(list, 1, &str_out) == true, "Failed to retrieve new string value");
    TEST_ASSERT(strcmp(str_out, "new_string") == 0, "New string value incorrect");
    // Ensure previous int type is gone
    TEST_ASSERT(plist_get_int(list, 1, &int_out) == false, "Old int type still accessible");

    // Test 3: Set Long at Index 2 (was double)
    plist_set_long(list, 2, 888L);
    TEST_ASSERT(plist_get_long(list, 2, &(long){0}) == true, "Failed to retrieve new long value");
    
    // Test 4: Out of Bounds
    plist_set_double(list, 3, 4.5);
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_SET_DOUBLE_OUT_OF_BOUNDS, "Incorrect error code for out of bounds set");

    // Test 5: NULL list input
    plist_set_int(NULL, 0, 1);
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_SET_INT_NULL_INPUT, "plist_set_int(NULL) failed to set correct error");

    plist_destroy(list);
    tests_passed++;
    return true;
}


/**
 * @brief Tests plist_remove and plist_empty.
 */
bool test_plist_remove_and_empty() {
    tests_run++;
    plist_t *list = plist_create(5);

    plist_add_int(list, 10);      // Index 0
    plist_add_str(list, "middle"); // Index 1
    plist_add_double(list, 3.14); // Index 2
    plist_add_long(list, 5L);     // Index 3
    
    // Test 1: Remove from the middle (Index 1 - "middle")
    plist_remove(list, 1);
    TEST_ASSERT(plist_get_size(list) == 3, "Size incorrect after middle remove");
    // Check that elements shifted (Index 2 should now be at Index 1)
    double dbl_out;
    TEST_ASSERT(plist_get_double(list, 1, &dbl_out) == true, "Element did not shift correctly");
    TEST_ASSERT(dbl_out == 3.14, "Shifted element value incorrect");

    // Test 2: Remove the last element (Index 2 - 5L)
    plist_remove(list, 2);
    TEST_ASSERT(plist_get_size(list) == 2, "Size incorrect after last element remove");
    
    // Test 3: Remove the first element (Index 0 - 10)
    plist_remove(list, 0);
    TEST_ASSERT(plist_get_size(list) == 1, "Size incorrect after first element remove");
    
    // Test 4: Out of Bounds remove
    plist_remove(list, 10);
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_REMOVE_OUT_OF_BOUNDS, "Incorrect error code for out of bounds remove");

    // Test 5: plist_empty
    plist_add_str(list, "temp"); // List has one int, one str
    plist_empty(list);
    TEST_ASSERT(plist_get_size(list) == 0, "plist_empty did not set size to 0");
    TEST_ASSERT(plist_get_capacity(list) == 5, "plist_empty should preserve capacity");
    // Test memory safety (should not crash on empty/destroy)
    plist_empty(NULL); // Should not crash

    plist_destroy(list);
    tests_passed++;
    return true;
}

/**
 * @brief Tests the plist_contains and pvar_equals logic.
 */
bool test_plist_contains() {
    tests_run++;
    plist_t *list = plist_create(5);
    
    plist_add_int(list, 42);
    plist_add_str(list, "test_string");
    plist_add_double(list, 0.125);
    
    // Test 1: Int exists
    pvar_t var_int_match = create_test_pvar_int(42);
    TEST_ASSERT(plist_contains(list, &var_int_match) == true, "plist_contains failed to find existing int");

    // Test 2: String exists
    // NOTE: Need to be careful with the pvar_t struct for string. The test pvar
    // is just for comparison, so it doesn't need strdup.
    pvar_t var_str_match = create_test_pvar_str("test_string");
    TEST_ASSERT(plist_contains(list, &var_str_match) == true, "plist_contains failed to find existing string");
    
    // Test 3: Double exists (using floating point comparison in pvar_equals)
    pvar_t var_double_match = {PVAR_TYPE_DOUBLE, {.d = 0.125}};
    TEST_ASSERT(plist_contains(list, &var_double_match) == true, "plist_contains failed to find existing double");

    // Test 4: Int does not exist
    pvar_t var_int_no_match = create_test_pvar_int(43);
    TEST_ASSERT(plist_contains(list, &var_int_no_match) == false, "plist_contains incorrectly found non-existent int");

    // Test 5: String does not exist
    pvar_t var_str_no_match = create_test_pvar_str("miss");
    TEST_ASSERT(plist_contains(list, &var_str_no_match) == false, "plist_contains incorrectly found non-existent string");

    // Test 6: Wrong type but same value (int vs long)
    pvar_t var_long_same_value = {PVAR_TYPE_LONG, {.l = 42L}};
    TEST_ASSERT(plist_contains(list, &var_long_same_value) == false, "plist_contains incorrectly matched int and long");
    
    // Test 7: NULL input
    TEST_ASSERT(plist_contains(NULL, &var_int_match) == false, "plist_contains(NULL list) failed to fail");
    TEST_ASSERT(pvars_errno == FAILURE_PLIST_CONTAINS_NULL_INPUT, "Incorrect error code for NULL list in contains");


    // Extra check: pvar_equals edge cases (needed to fully cover pvar_equals)
    pvar_t var_none_a = {PVAR_TYPE_NONE, {0}};
    pvar_t var_none_b = {PVAR_TYPE_NONE, {0}};
    TEST_ASSERT(pvar_equals(&var_none_a, &var_none_b) == true, "pvar_equals failed for PVAR_TYPE_NONE");
    
    plist_destroy(list);
    tests_passed++;
    return true;
}

/**
 * @brief Tests accessor functions for size and capacity.
 */
bool test_plist_metadata() {
    tests_run++;
    plist_t *list = plist_create(5);

    // Test 1: Initial state
    TEST_ASSERT(plist_get_size(list) == 0, "Initial size incorrect");
    TEST_ASSERT(plist_get_capacity(list) == 5, "Initial capacity incorrect");

    plist_add_int(list, 1);
    plist_add_int(list, 2);

    // Test 2: After adding
    TEST_ASSERT(plist_get_size(list) == 2, "Size after add incorrect");
    TEST_ASSERT(plist_get_capacity(list) == 5, "Capacity after add incorrect");

    // Test 3: NULL list checks
    TEST_ASSERT(plist_get_size(NULL) == 0, "plist_get_size(NULL) should return 0");
    TEST_ASSERT(plist_get_capacity(NULL) == 0, "plist_get_capacity(NULL) should return 0");

    plist_destroy(list);
    tests_passed++;
    return true;
}

/* --- Main Test Runner --- */

int main(void) {
    printf("🚀 Starting pvars Library Test Suite...\n");

    // Run all tests
    bool result = test_plist_creation_and_destruction() &&
                  test_plist_add_and_resize() &&
                  test_plist_get_and_type_safety() &&
                  test_plist_set_functions() &&
                  test_plist_remove_and_empty() &&
                  test_plist_contains() &&
                  test_plist_metadata();

    printf("\n--- Test Summary ---\n");
    if (tests_passed == tests_run) {
        printf("✅ ALL %d TESTS PASSED!\n", tests_passed);
    } else {
        printf("❌ %d/%d TESTS PASSED. See 'FAIL' messages above.\n", tests_passed, tests_run);
    }

    // You might want to also call plist_print() on a successful list creation 
    // to visually verify the output, though it's not a formal test.
    /*
    plist_t *demo = plist_create(3);
    plist_add_int(demo, 10);
    plist_add_str(demo, "hello");
    plist_add_double(demo, 1.23);
    printf("\nVisual check (plist_print): ");
    plist_print(demo);
    plist_destroy(demo);
    */

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
