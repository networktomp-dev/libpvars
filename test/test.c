#define _POSIX_C_SOURCE 200809L

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>

#include"pvars.h"
#include"pdict.h"
#include"perrno.h"

/******************************************************************************
 * GLOBAL TEST STATE AND ERROR DEFINITIONS
 ******************************************************************************/

/* Global thread-local error variable from perrno.c */
extern __thread int pvars_errno;

/* Function to retrieve the error message from perrno.c */
extern const char *perror_message(void);

/* Cleanup function to destroy pvar_t. Defined in pvars.c, needed for temporary pvar in test. */
void pvar_destroy_internal(pvar_t *pvar);

/* Counters for test results */
static int tests_run = 0;
static int tests_failed = 0;

/******************************************************************************
 * HELPER MACROS (To make tests clean and readable)
 ******************************************************************************/

/* Start of a test function */
#define TEST_START(name) \
	printf("TEST:\t\t%s... ", name); \
	fflush(stdout); \
	tests_run++;

/* End of a test function */
#define TEST_END() \
	printf("SUCCESS\n"); \
	pvars_errno = PERRNO_CLEAR; \
	return;

/* Macro for simple boolean assertion */
#define ASSERT_TRUE(condition, message) \
	if (!(condition)) { \
		printf("FAIL\n"); \
		printf("\tERROR:\t\t[%s] Assertion failed: %s (in %s)\n", __func__, message, #condition); \
		tests_failed++; \
		return; \
	}

/* Macro for integer equality assertion */
#define ASSERT_EQUALS_INT(expected, actual, message) \
	if ((expected) != (actual)) { \
		printf("FAIL\n"); \
		printf("\tERROR:\t\t[%s] Assertion failed: %s (Expected: %ld, Actual: %ld)\n", __func__, message, (long)expected, (long)actual); \
		tests_failed++; \
		return; \
	}

/* Macro for checking pvars_errno after a function call */
#define ASSERT_ERRNO(expected_errno, func_call) \
	pvars_errno = PERRNO_CLEAR; \
	func_call; \
	ASSERT_EQUALS_INT(expected_errno, pvars_errno, "pvars_errno check failed");

/* Macro for checking floating point equality within a tolerance (epsilon) */
#define ASSERT_EQUALS_FLOAT(expected, actual, tolerance, message) \
	if (fabs((double)(expected) - (double)(actual)) > (double)(tolerance)) { \
		printf("FAIL\n"); \
		printf("\tERROR:\t\t[%s] Assertion failed: %s (Expected: %f, Actual: %f, Tolerance: %f)\n", __func__, message, (double)expected, (double)actual, (double)tolerance); \
		tests_failed++; \
		return; \
	}

/******************************************************************************
 * PLIST TEST FUNCTIONS
 ******************************************************************************/

/**
 * @brief Tests basic creation, size, and destruction of plist_t.
 */
void test_plist_creation_and_destruction(void)
{
	TEST_START("List Creation/Destruction");

	/* Test basic creation */
	plist_t *list = plist_create(10);
	ASSERT_TRUE(list != NULL, "plist_create failed");
	ASSERT_EQUALS_INT(0, plist_get_size(list), "Initial size must be 0");
	ASSERT_EQUALS_INT(10, plist_get_capacity(list), "Initial capacity must be 10");

	/* Test failure on invalid capacity */
	ASSERT_ERRNO(FAILURE_PLIST_CREATE_CAPACITY_OUT_OF_BOUNDS, plist_create(0));

	/* Test destruction */
	plist_destroy(list);

	/* Test creation with smaller capacity */
	plist_t *list2 = plist_create(1);
	ASSERT_TRUE(list2 != NULL, "plist_create failed with capacity 1");
	ASSERT_EQUALS_INT(1, plist_get_capacity(list2), "Capacity 1 check");
	plist_destroy(list2);

	TEST_END();
}

/**
 * @brief Tests adding all primitive types to a list.
 */
void test_plist_add_primitives(void)
{
	TEST_START("List Add Primitives");
	plist_t *list = plist_create(1);

	/* 1. Add String */
	plist_add_str(list, "Hello");
	ASSERT_EQUALS_INT(1, plist_get_size(list), "Size after add_str");

	/* 2. Add Integer */
	plist_add_int(list, 10);
	ASSERT_EQUALS_INT(2, plist_get_size(list), "Size after add_int");

	/* 3. Add Double */
	plist_add_double(list, 3.14159);
	ASSERT_EQUALS_INT(3, plist_get_size(list), "Size after add_double");

	/* 4. Add Long */
	plist_add_long(list, 9876543210L);
	ASSERT_EQUALS_INT(4, plist_get_size(list), "Size after add_long");

	/* 5. Add Float */
	plist_add_float(list, 0.5f);
	ASSERT_EQUALS_INT(5, plist_get_size(list), "Size after add_float");

	/* Capacity check (should have resized) */
	ASSERT_TRUE(plist_get_capacity(list) >= 5, "Capacity check after resize");

	plist_destroy(list);
	TEST_END();
}

/**
 * @brief Tests retrieval and type checking for all primitive types.
 */
void test_plist_get_primitives(void)
{
	TEST_START("List Get Primitives");
	plist_t *list = plist_create(5);

	/* Add test data */
	plist_add_str(list, "Testing Get");
	plist_add_int(list, 42);
	plist_add_double(list, 1.2345);
	plist_add_long(list, 100000000000L);
	plist_add_float(list, 2.71828f);

	/* 1. Get String */
	char *s_val = NULL;
	ASSERT_TRUE(plist_get_str(list, 0, &s_val), "Get string success check");
	ASSERT_EQUALS_INT(0, pvars_errno, "Get string errno check");
	ASSERT_TRUE(strcmp(s_val, "Testing Get") == 0, "String value check");

	/* 2. Get Integer */
	int i_val = 0;
	ASSERT_TRUE(plist_get_int(list, 1, &i_val), "Get int success check");
	ASSERT_EQUALS_INT(42, i_val, "Int value check");

	/* 3. Get Double */
	double d_val = 0.0;
	ASSERT_TRUE(plist_get_double(list, 2, &d_val), "Get double success check");
	ASSERT_EQUALS_FLOAT(1.2345, d_val, 0.00001, "Double value check");

	/* 4. Get Long */
	long l_val = 0L;
	ASSERT_TRUE(plist_get_long(list, 3, &l_val), "Get long success check");
	ASSERT_EQUALS_INT(100000000000L, l_val, "Long value check");

	/* 5. Get Float */
	float f_val = 0.0f;
	ASSERT_TRUE(plist_get_float(list, 4, &f_val), "Get float success check");
	ASSERT_EQUALS_FLOAT(2.71828f, f_val, 0.0001f, "Float value check");

	/* Test failure conditions (Out of bounds) */
	ASSERT_ERRNO(FAILURE_PLIST_GET_STR_OUT_OF_BOUNDS, plist_get_str(list, 99, &s_val));
	ASSERT_ERRNO(FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS, plist_get_int(list, 99, &i_val));

	/* Test failure conditions (Wrong type) */
	ASSERT_ERRNO(FAILURE_PLIST_GET_INT_WRONG_TYPE, plist_get_int(list, 0, &i_val));
	ASSERT_ERRNO(FAILURE_PLIST_GET_STR_WRONG_TYPE, plist_get_str(list, 1, &s_val));

	plist_destroy(list);
	TEST_END();
}

/**
 * @brief Tests setting values for all primitive types.
 */
void test_plist_set_primitives(void)
{
	TEST_START("List Set Primitives");
	plist_t *list = plist_create(5);

	/* Initialize with placeholder elements */
	for (int i = 0; i < 5; i++) {
		plist_add_int(list, i);
	}

	/* 1. Set String */
	plist_set_str(list, 0, "New String");
	char *s_val = NULL;
	ASSERT_TRUE(plist_get_str(list, 0, &s_val), "Set string success check");
	ASSERT_TRUE(strcmp(s_val, "New String") == 0, "Set string value check");

	/* 2. Set Integer */
	plist_set_int(list, 1, -99);
	int i_val = 0;
	ASSERT_TRUE(plist_get_int(list, 1, &i_val), "Set int success check");
	ASSERT_EQUALS_INT(-99, i_val, "Set int value check");

	/* 3. Set Double */
	plist_set_double(list, 2, 9.99);
	double d_val = 0.0;
	ASSERT_TRUE(plist_get_double(list, 2, &d_val), "Set double success check");
	ASSERT_EQUALS_FLOAT(9.99, d_val, 0.00001, "Set double value check");

	/* Test failure conditions (Out of bounds) */
	ASSERT_ERRNO(FAILURE_PLIST_SET_STR_OUT_OF_BOUNDS, plist_set_str(list, 99, "Invalid"));

	/* Test failure condition (Set NULL string) */
	ASSERT_ERRNO(FAILURE_PLIST_SET_STR_NULL_STRING_INPUT, plist_set_str(list, 3, NULL));

	plist_destroy(list);
	TEST_END();
}

/**
 * @brief Tests list operations like empty, contains, and nested lists.
 */
void test_plist_nested_and_utility(void)
{
	TEST_START("List Nested/Utility");
	plist_t *list = plist_create(1);

	/* Create a nested list */
	plist_t *nested_list = plist_create(2);
	plist_add_str(nested_list, "Nested A");
	plist_add_int(nested_list, 100);

	/* Add the nested list to the main list */
	plist_add_list(list, nested_list);
	ASSERT_EQUALS_INT(1, plist_get_size(list), "Size after add_list");

	/* Add a string for contains test */
	plist_add_str(list, "Test Contains");

	/* 1. Get Nested List and verify it's a deep copy */
	plist_t *retrieved_list = NULL;
	ASSERT_TRUE(plist_get_list(list, 0, &retrieved_list), "Get nested list success check");
	ASSERT_EQUALS_INT(2, plist_get_size(retrieved_list), "Retrieved list size check");

	/* Modify the original nested list (if the retrieved copy is deep, it shouldn't change) */
	plist_add_str(nested_list, "Should NOT appear in retrieved_list");
	ASSERT_EQUALS_INT(2, plist_get_size(retrieved_list), "Deep copy check failure: size changed");

	/* Destroy the original nested list (the copy in 'list' should survive) */
	plist_destroy(nested_list);

	/* 2. Test plist_contains */
	pvar_t element_str;
	element_str.type = PVAR_TYPE_STRING;
	element_str.data.s = strdup("Test Contains"); /* Must be copied, but we only check equality */

	ASSERT_TRUE(plist_contains(list, &element_str), "Contains check (String) must be true");

	pvar_destroy_internal(&element_str); /* Clean up the temporary pvar */

	/* 3. Test plist_empty */
	plist_empty(list);
	ASSERT_EQUALS_INT(0, plist_get_size(list), "Size after empty");
	ASSERT_TRUE(plist_get_capacity(list) > 0, "Capacity after empty should remain");

	plist_destroy(list);
	TEST_END();
}

/**
 * @brief Tests deep copy functionality of plist_copy.
 */
void test_plist_copy(void)
{
	TEST_START("List Deep Copy");

	plist_t *original = plist_create(10);
	plist_add_str(original, "Original String");
	plist_add_int(original, 10);

	/* Nested list for deep copy check */
	plist_t *nested = plist_create(1);
	plist_add_int(nested, 500);
	plist_add_list(original, nested);
	plist_destroy(nested); /* Destroy the temporary, original nested list */

	plist_t *copy = plist_copy(original);
	ASSERT_TRUE(copy != NULL, "plist_copy failed");
	ASSERT_EQUALS_INT(plist_get_size(original), plist_get_size(copy), "Copy size mismatch");

	/* 1. Verify Deep Copy of String (Change original, copy must be stable) */
	plist_set_str(original, 0, "New Original String");
	char *s_val = NULL;
	plist_get_str(copy, 0, &s_val);
	ASSERT_TRUE(strcmp(s_val, "Original String") == 0, "String was not deep copied");

	/* 2. Verify Deep Copy of Nested List */
	plist_t *copy_nested = NULL;
	plist_get_list(copy, 2, &copy_nested);
	plist_add_int(original, 999); /* Add to original (not the nested part) */
	ASSERT_EQUALS_INT(1, plist_get_size(copy_nested), "Nested list was not deep copied (size error)");

	plist_destroy(original);
	plist_destroy(copy);
	TEST_END();
}

/******************************************************************************
 * PDICT TEST FUNCTIONS
 ******************************************************************************/

/**
 * @brief Tests basic creation, size, and destruction of pdict_t.
 */
void test_pdict_creation_and_destruction(void)
{
	TEST_START("Dict Creation/Destruction");

	/* Test basic creation */
	pdict_t *dict = pdict_create(8);
	ASSERT_TRUE(dict != NULL, "pdict_create failed");
	ASSERT_EQUALS_INT(0, pdict_get_size(dict), "Initial size must be 0");
	ASSERT_EQUALS_INT(8, pdict_get_capacity(dict), "Initial capacity must be 8");

	/* Test failure on invalid capacity */
	ASSERT_ERRNO(FAILURE_PDICT_CREATE_CAPACITY_OUT_OF_BOUNDS, pdict_create(0));

	/* Test destruction */
	pdict_destroy(dict);

	TEST_END();
}

/**
 * @brief Tests adding and overwriting all types in a dictionary.
 */
void test_pdict_add_and_overwrite(void)
{
	TEST_START("Dict Add and Overwrite");
	pdict_t *dict = pdict_create(4);

	/* 1. Initial Adds */
	plist_t *temp_list = plist_create(1);
	plist_add_int(temp_list, 100);

	pdict_add_str(dict, "name", "John");
	pdict_add_int(dict, "age", 30);
	pdict_add_double(dict, "pi", 3.14);
	pdict_add_list(dict, "items", temp_list);

	ASSERT_EQUALS_INT(4, pdict_get_size(dict), "Size after initial adds");
	plist_destroy(temp_list); /* The dict owns a copy, so we destroy the original */

	/* 2. Overwrite Tests (using different types to ensure pvar_destroy_internal is called) */

	/* Overwrite str with int */
	pdict_add_int(dict, "name", 123);
	ASSERT_EQUALS_INT(4, pdict_get_size(dict), "Size after overwriting str with int");

	/* Overwrite int with float */
	pdict_add_float(dict, "age", 30.5f);
	ASSERT_EQUALS_INT(4, pdict_get_size(dict), "Size after overwriting int with float");

	/* Overwrite double with a new list (deep copy check) */
	plist_t *new_list = plist_create(1);
	plist_add_str(new_list, "New Item");
	pdict_add_list(dict, "pi", new_list);

	ASSERT_EQUALS_INT(4, pdict_get_size(dict), "Size after overwriting double with list");
	plist_destroy(new_list);

	/* 3. Error Checks */
	ASSERT_ERRNO(FAILURE_PDICT_ADD_STR_NULL_INPUT_DICT, pdict_add_str(NULL, "key", "val"));
	ASSERT_ERRNO(FAILURE_PDICT_ADD_STR_NULL_INPUT_KEY, pdict_add_str(dict, NULL, "val"));
	ASSERT_ERRNO(FAILURE_PDICT_ADD_STR_NULL_INPUT_VALUE, pdict_add_str(dict, "key", NULL));

	pdict_destroy(dict);
	TEST_END();
}

/**
 * @brief Tests the pdict_remove and pdict_empty functions.
 */
void test_pdict_remove_and_empty(void)
{
	TEST_START("Dict Remove and Empty");
	pdict_t *dict = pdict_create(16);

	/* Add elements */
	pdict_add_str(dict, "key1", "val1");
	pdict_add_int(dict, "key2", 2);
	pdict_add_double(dict, "key3", 3.0);
	ASSERT_EQUALS_INT(3, pdict_get_size(dict), "Initial size for remove test");

	/* 1. Remove an existing key */
	pdict_remove(dict, "key2");
	ASSERT_EQUALS_INT(2, pdict_get_size(dict), "Size after removing key2");

	/* 2. Try to remove a non-existent key (should not change size) */
	pdict_remove(dict, "nonexistent");
	ASSERT_EQUALS_INT(2, pdict_get_size(dict), "Size after removing nonexistent key");

	/* 3. Remove the rest of the keys */
	pdict_remove(dict, "key1");
	pdict_remove(dict, "key3");
	ASSERT_EQUALS_INT(0, pdict_get_size(dict), "Size after removing all keys");

	/* 4. Repopulate for pdict_empty test */
	pdict_add_str(dict, "reset", "val");
	ASSERT_EQUALS_INT(1, pdict_get_size(dict), "Size before empty");

	/* 5. Test pdict_empty */
	pdict_empty(dict);
	ASSERT_EQUALS_INT(0, pdict_get_size(dict), "Size after empty");
	ASSERT_EQUALS_INT(16, pdict_get_capacity(dict), "Capacity after empty");

	pdict_destroy(dict);
	TEST_END();
}

/**
 * @brief Tests the error reporting mechanism.
 */
void test_perrno_message(void)
{
	TEST_START("Error Reporting");
	pvars_errno = PERRNO_CLEAR;
	
	/* Test SUCCESS message */
	ASSERT_TRUE(strcmp(perror_message(), "SUCCESS: Operation was successful") == 0, "SUCCESS message check");

	/* Set an arbitrary failure code and check the message */
	pvars_errno = FAILURE_PLIST_ADD_REALLOC_FAILED;
	const char *expected = "FAILURE: realloc() failed to allocated memory when resizing list->elements in list add/set function";
	ASSERT_TRUE(strcmp(perror_message(), expected) == 0, "Realloc error message check");

	/* Clear error */
	pvars_errno = PERRNO_CLEAR;
	ASSERT_EQUALS_INT(PERRNO_CLEAR, pvars_errno, "Error clear check");

	TEST_END();
}

/******************************************************************************
 * MAIN DRIVER
 ******************************************************************************/

void (*test_functions[])(void) = {
	test_plist_creation_and_destruction,
	test_plist_add_primitives,
	test_plist_get_primitives,
	test_plist_set_primitives,
	test_plist_nested_and_utility,
	test_plist_copy,
	test_pdict_creation_and_destruction,
	test_pdict_add_and_overwrite,
	test_pdict_remove_and_empty,
	test_perrno_message,
	NULL
};

/**
 * @brief Main function to run all tests.
 */
int main(void)
{
	void (**func)(void);
	printf("--- Running libpvars Test Suite ---\n");
	printf("Tests are structured for future addition and easy maintenance.\n\n");

	for (func = test_functions; *func != NULL; func++) {
		(**func)();
	}

	printf("\n--- Test Summary ---\n");
	if (tests_failed == 0) {
		printf("RESULT:\t\tALL %d tests passed successfully!\n", tests_run);
		return EXIT_SUCCESS;
	} else {
		printf("RESULT:\t\t%d/%d tests FAILED.\n", tests_failed, tests_run);
		return EXIT_FAILURE;
	}
}


