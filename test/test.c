#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Include your public API headers
#include "plist.h"
#include "pdict.h"
#include "perrno.h"

// Define a global counter for tests to track successes and failures
static int tests_run = 0;
static int tests_failed = 0;

/**
 * @brief Helper to print the current error message and a failure indicator.
 * @param test_name The name of the test that failed.
 * @param expected_err The expected error code (for debugging).
 */
void print_error_and_fail(const char *test_name, int expected_err)
{
    fprintf(stderr, "\n--- FAILED TEST: %s ---\n", test_name);
    fprintf(stderr, "    pvars_errno: %d (Expected: %d)\n", pvars_errno, expected_err);
    fprintf(stderr, "    Error Message: %s\n", perror_message());
    tests_failed++;
}

/**
 * @brief Macro for checking test conditions.
 * If the condition fails, it calls the error handler and returns.
 */
#define ASSERT(test_name, condition, expected_err) \
    do { \
        tests_run++; \
        if (!(condition)) { \
            print_error_and_fail(test_name, expected_err); \
            return; \
        } \
    } while (0)

/**
 * @brief Tests basic plist_t functionality: creation, adding, access, and cleanup.
 */
void test_list_operations(void)
{
    printf("--- Testing List (plist_t) Operations ---\n");
    plist_t *list = plist_create(10);
    ASSERT("List Create", list != NULL && plist_get_capacity(list) >= 10, SUCCESS);
    ASSERT("List Size Initial", plist_get_size(list) == 0, SUCCESS);

    // 1. Add various data types
    plist_add_str(list, "Hello List");
    ASSERT("List Add Str", plist_get_size(list) == 1 && pvars_errno == SUCCESS, FAILURE_PLIST_ADD_NULL_INPUT_LIST);

    plist_add_int(list, 42);
    plist_add_double(list, 3.14159);
    plist_add_float(list, 2.718f);
    ASSERT("List Add All Types", plist_get_size(list) == 4, SUCCESS);
    
    printf("List after additions:\n");
    plist_print(list);

    // 2. Test Retrieval (Get)
    char *s_val;
    int i_val;
    double d_val;
    float f_val;

    // Get string
    bool success = plist_get_str(list, 0, &s_val);
    ASSERT("List Get Str", success && strcmp(s_val, "Hello List") == 0, SUCCESS);

    // Get int
    success = plist_get_int(list, 1, &i_val);
    ASSERT("List Get Int", success && i_val == 42, FAILURE_PLIST_GET_INT_WRONG_TYPE);
    
    // Get double
    success = plist_get_double(list, 2, &d_val);
    ASSERT("List Get Double", success && d_val > 3.14 && d_val < 3.15, FAILURE_PLIST_GET_DOUBLE_WRONG_TYPE);

    // Get float
    success = plist_get_float(list, 3, &f_val);
    ASSERT("List Get Float", success && f_val > 2.71 && f_val < 2.72, FAILURE_PLIST_GET_FLOAT_WRONG_TYPE);

    // 3. Test Modification (Set)
    plist_set_int(list, 1, 999);
    success = plist_get_int(list, 1, &i_val);
    ASSERT("List Set Int", success && i_val == 999, SUCCESS);

    // 4. Test error case: Out of bounds access
    plist_get_int(list, 100, &i_val);
    ASSERT("List Get OOB Error", pvars_errno == FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS, FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS);
    pvars_errno = PERRNO_CLEAR; // Clear error for next test

    // 5. Cleanup
    plist_destroy(list);
    ASSERT("List Destroy", true, SUCCESS); // If we reached here, destroy didn't crash
    printf("List tests complete (destroyed).\n\n");
}

/**
 * @brief Tests basic pdict_t functionality: creation, adding, updating, and cleanup.
 */
void test_dict_operations(void)
{
    printf("--- Testing Dictionary (pdict_t) Operations ---\n");
    pdict_t *dict = pdict_create(5);
    ASSERT("Dict Create", dict != NULL, FAILURE_PDICT_CREATE_NEW_DICT_MALLOC_FAILED);

    // 1. Add new key-value pairs
    pdict_add_str(dict, strdup("Name"), "Alice");
    pdict_add_int(dict, strdup("Age"), 30);
    pdict_add_double(dict, strdup("Pi"), 3.14159);
    pdict_add_float(dict, strdup("e"), 2.718f);

    // Note: Since there are no public GET functions yet, we mainly test additions and updates
    // ASSERTs here mainly check that no internal memory failures occurred.
    ASSERT("Dict Add Str", pvars_errno == SUCCESS, pvars_errno);
    ASSERT("Dict Add Int", pvars_errno == SUCCESS, pvars_errno);
    ASSERT("Dict Add Double", pvars_errno == SUCCESS, pvars_errno);
    ASSERT("Dict Add Float", pvars_errno == SUCCESS, pvars_errno);
    
    // 2. Test Update (add to existing key)
    pdict_add_int(dict, strdup("Age"), 31);
    ASSERT("Dict Update Int", pvars_errno == SUCCESS, pvars_errno);
    
    // 3. Test destruction (visual check, should not crash)
    pdict_destroy(dict);
    ASSERT("Dict Destroy", true, SUCCESS);
    printf("Dictionary tests complete (destroyed).\n\n");
}


/**
 * @brief Main test execution function.
 */
int main(void)
{
    printf("======================================\n");
    printf(" Starting libpvars API Test Suite\n");
    printf("======================================\n\n");

    test_list_operations();
    test_dict_operations();

    printf("======================================\n");
    printf(" Total Tests Run: %d\n", tests_run);
    printf(" Total Tests Failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf(" RESULT: ALL TESTS PASSED! 🎉\n");
    } else {
        printf(" RESULT: %d TEST(S) FAILED! ❌\n", tests_failed);
    }
    printf("======================================\n");

    return tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
