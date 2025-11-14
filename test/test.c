#include<stdio.h>
#include<math.h>
#include<float.h>

#include"pvars.h"
#include"pvars_internal.h" 
#include"plist_internal.h" 
#include"pdict_internal.h"
#include"perrno.h"

#define ASSERT_TRUE(condition, message) \
	do { \
		if (!(condition)) { \
			fprintf(stderr, "FAIL: %s - %s\n", __func__, message); \
			return 1; \
	} \
} while (0)

/* End of a test function */
#define TEST_END() \
	pvars_errno = PERRNO_CLEAR; \
	return 0;

/* ---------------------- */
/* Test 1: plist_create() */
/* ---------------------- */
int test_plist_create(void)
{	
	/* Create and destroy the same list several times */
	/* Index 0 */
	plist_t *list = plist_create(0);
	ASSERT_TRUE(list == NULL, "Did not get the expected NULL.");
	
	/* Index 1 */
	list = plist_create(1);
	ASSERT_TRUE(list != NULL, "List allocation failed at index 1.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0 at index 1.");
	ASSERT_TRUE(list->capacity == 1, "Initial list capacity should be 1 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1.");
	
	plist_destroy(list);
	
	/* Index 2 */
	list = plist_create(1024);
	ASSERT_TRUE(list != NULL, "List allocation failed at index 2.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0 at index 2.");
	ASSERT_TRUE(list->capacity == 1024, "Initial list capacity should be 1024 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2.");

	plist_destroy(list);
	
	/* Index 3 */
	list = plist_create(9999);
	ASSERT_TRUE(list != NULL, "List allocation failed.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0 at index 3.");
	ASSERT_TRUE(list->capacity == 9999, "Initial list capacity should be 9999 at index 3.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 3.");

	plist_destroy(list);
	
	TEST_END();
}

/* ----------------------- */
/* Test 2: plist_add_str() */
/* ----------------------- */
int test_plist_add_str(void)
{
	plist_t *list = plist_create(1);
	
	/* Index 0 */
	plist_add_str(list, "libpvars");
	ASSERT_TRUE(list->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0.");
	ASSERT_TRUE(strcmp(list->elements[0].data.s, "libpvars") == 0, "strings are not equal at index 0.");
	
	/* Index 1 */
	plist_add_str(list, "test suite");
	ASSERT_TRUE(list->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1.");
	ASSERT_TRUE(strcmp(list->elements[1].data.s, "test suite") == 0, "strings are not equal at index 1.");
	
	/* Index 2 */
	plist_add_str(list, "API");
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2.");
	ASSERT_TRUE(strcmp(list->elements[2].data.s, "API") == 0, "strings are not equal at index 2.");
	
	/* Test for a graceful fail when NULL is passed in */
	/* Index 3 */
	char *null_string = NULL;
	plist_add_str(list, null_string);
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 3."); /* String count should remain unchanged since the last successful add */
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 3.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_ADD_STR_NULL_STRING_INPUT, "Expected FAILURE_PLIST_ADD_STR_NULL_STRING_INPUT at index 3.");
	
	/* Index 4 */
	plist_t *null_list = NULL;
	plist_add_str(null_list, "library");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_ADD_STR_NULL_INPUT_LIST, "Expected FAILURE_PLIST_ADD_STR_NULL_INPUT at index 4.");

	plist_destroy(list);
	
	TEST_END();
}

/* ----------------------- */
/* Test 3: plist_add_int() */
/* ----------------------- */
int test_plist_add_int(void)
{
	plist_t *list = plist_create(1);
	
	/* Index 0 */
	plist_add_int(list, 0);
	ASSERT_TRUE(list->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0");
	ASSERT_TRUE(list->elements[0].data.i == 0, "expected int to equal 0");
	
	/* Index 1 */
	plist_add_int(list, 1024);
	ASSERT_TRUE(list->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1");
	ASSERT_TRUE(list->elements[1].data.i == 1024, "expected int to equal 1024");
	
	/* Index 2 */
	plist_add_int(list, -9999);
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2");
	ASSERT_TRUE(list->elements[2].data.i == -9999, "expected int to equal -9999");
	
	/* Index 3 */
	plist_t *null_list = NULL;
	plist_add_int(null_list, 23567);
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_ADD_INT_NULL_INPUT, "Expected FAILURE_PLIST_ADD_INT_NULL_INPUT at index 3.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* ------------------------ */
/* Test 4: plist_add_long() */
/* ------------------------ */
int test_plist_add_long(void)
{
	plist_t *list = plist_create(1);
	
	/* Index 0 */
	plist_add_long(list, 0);
	ASSERT_TRUE(list->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0");
	ASSERT_TRUE(list->elements[0].data.l == 0, "expected long to equal 0");
	
	/* Index 1 */
	plist_add_long(list, 1024);
	ASSERT_TRUE(list->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1");
	ASSERT_TRUE(list->elements[1].data.l == 1024, "expected long to equal 1024");
	
	/* Index 2 */
	plist_add_long(list, -9999);
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2");
	ASSERT_TRUE(list->elements[2].data.l == -9999, "expected long to equal -9999");
	
	/* Index 3 */
	plist_t *null_list = NULL;
	plist_add_long(null_list, 23567);
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_ADD_LONG_NULL_INPUT, "Expected FAILURE_PLIST_ADD_LONG_NULL_INPUT at index 3.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* -------------------------- */
/* Test 5: plist_add_double() */
/* -------------------------- */
int test_plist_add_double(void)
{
	plist_t *list = plist_create(1);
	
	/* Index 0 */
	plist_add_double(list, 0.2);
	ASSERT_TRUE(list->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0");
	ASSERT_TRUE(fabs(list->elements[0].data.d - 0.2) < (DBL_EPSILON * fmax(fabs(list->elements[0].data.d), fabs(0.2))), "expected double to equal 0.2");
	
	/* Index 1 */
	plist_add_double(list, 1024.4);
	ASSERT_TRUE(list->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1");
	ASSERT_TRUE(fabs(list->elements[1].data.d - 1024.4) < (DBL_EPSILON * fmax(fabs(list->elements[1].data.d), fabs(1024.4))), "expected double to equal 1024.4");
	
	/* Index 2 */
	plist_add_double(list, -9999.3);
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2");
	ASSERT_TRUE(fabs(list->elements[2].data.d - (-9999.3)) < (DBL_EPSILON * fmax(fabs(list->elements[2].data.d), fabs(-9999.3))), "expected double to equal -9999.3");
	
	plist_destroy(list);
	
	TEST_END();
}

/* ------------------------- */
/* Test 6: plist_add_float() */
/* ------------------------- */
int test_plist_add_float(void)
{
	plist_t *list = plist_create(1);
	
	/* Index 0 */
	plist_add_float(list, 0.2);
	ASSERT_TRUE(list->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0");
	ASSERT_TRUE(fabsf(list->elements[0].data.f - 0.2f) < FLT_EPSILON, "expected float to equal 0.2");
	
	/* Index 1 */
	plist_add_float(list, 1024.4);
	ASSERT_TRUE(list->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1");
	ASSERT_TRUE(fabsf(list->elements[1].data.f - 1024.4f) < FLT_EPSILON, "expected float to equal 1024.4");
	
	/* Index 2 */
	plist_add_float(list, -9999.3);
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2");
	ASSERT_TRUE(fabsf(list->elements[2].data.f - (-9999.3f)) < FLT_EPSILON, "expected float to equal -9999.3");
	
	plist_destroy(list);
	
	TEST_END();
}

/* ------------------------ */
/* Test 7: plist_add_list() */
/* ------------------------ */
int test_plist_add_list(void)
{
	plist_t *list_main = plist_create(1);
	plist_t *list_child1 = plist_create(1);
	plist_t *list_child2 = plist_create(1);
	plist_t *list_child3 = plist_create(1);
	
	plist_add_int(list_child1, 0);
	plist_add_str(list_child1, "libpvars");
	plist_add_float(list_child1, 0.2);
	plist_add_long(list_child1, 0);
	plist_add_double(list_child1, 0.2);
	
	plist_add_str(list_child2, "test suite");
	plist_add_float(list_child2, 1024.4);
	plist_add_int(list_child2, -9999);
	plist_add_long(list_child2, -1024);
	plist_add_double(list_child2, 1024.4);
	
	plist_add_str(list_child3, "API");
	plist_add_int(list_child3, 1024);
	plist_add_float(list_child3, -9999.3);
	plist_add_long(list_child3, -9999);
	plist_add_double(list_child3, -9999.3);
	
	/* Index 0 */
	plist_add_list(list_main, list_child1);
	ASSERT_TRUE(list_main->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list_main->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0.");
	
	/* Index 1 */
	plist_add_list(list_main, list_child2);
	ASSERT_TRUE(list_main->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list_main->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1.");
	
	/* Index 2 */
	plist_add_list(list_main, list_child3);
	ASSERT_TRUE(list_main->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list_main->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2.");
	
	plist_destroy(list_child1);
	plist_destroy(list_child2);
	plist_destroy(list_child3);

	plist_destroy(list_main);
	
	TEST_END();
}

/* ------------------------ */
/* Test 8: plist_add_dict() */
/* ------------------------ */
int test_plist_add_dict(void)
{
	plist_t *list = plist_create(1);
	pdict_t *dict1 = pdict_create(1);
	pdict_t *dict2 = pdict_create(1);
	pdict_t *dict3 = pdict_create(1);
	
	/* Index 0 */
	plist_add_dict(list, dict1);
	ASSERT_TRUE(list->count == 1, "Expected a count of 1 at index 0.");
	ASSERT_TRUE(list->capacity == 1, "Expected a capacity of 1 at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0.");
	
	/* Index 1 */
	plist_add_dict(list, dict2);
	ASSERT_TRUE(list->count == 2, "Expected a count of 2 at index 1.");
	ASSERT_TRUE(list->capacity == 2, "Expected a capacity of 2 at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1.");
	
	/* Index 2 */
	plist_add_dict(list, dict3);
	ASSERT_TRUE(list->count == 3, "Expected a count of 3 at index 2.");
	ASSERT_TRUE(list->capacity == 4, "Expected a capacity of 4 at index 2.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2.");

	pdict_destroy(dict1);
	pdict_destroy(dict2);
	pdict_destroy(dict3);
	
	plist_destroy(list);
	
	TEST_END();
}

/* ----------------------- */
/* Test 9: plist_get_str() */
/* ----------------------- */
int test_plist_get_str(void)
{
	plist_t *list = plist_create(1);
	
	plist_add_str(list, "libpvars");
	plist_add_str(list, "test suite");
	plist_add_int(list, 12);
	plist_add_str(list, "API");
	
	char *string = NULL;
	bool result;
	
	/* Index 0 */
	result = plist_get_str(list, 0, &string);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(string != NULL, "Failed to get string at index 0.");
	ASSERT_TRUE(strcmp(string, "libpvars") == 0, "Expected string to match at index 0.");
	string[3] = '!'; // Since this is a deep copy, the following tests that the strings are not equal
	ASSERT_TRUE(strcmp(string, "libpvars") != 0, "Expected string NOT to match at index 0.");
	free(string);
	
	/* Index 1 */
	result = plist_get_str(list, 1, &string);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(string != NULL, "Failed to get string at index 1.");
	ASSERT_TRUE(strcmp(string, "test suite") == 0, "Expected string to match at index 1.");
	string[3] = '!';
	ASSERT_TRUE(strcmp(string, "test suite") != 0, "Expected string NOT to match at index 1.");
	free(string);
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_str(list, 2, &string);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno != SUCCESS, "Expected pvars_errno != SUCCESS at index 2.");
	
	/* Index 3 */
	result = plist_get_str(list, 3, &string);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(string != NULL, "Failed to get string at index 3.");
	ASSERT_TRUE(strcmp(string, "API") == 0, "Expected string to match at index 3.");
	string[2] = '!';
	ASSERT_TRUE(strcmp(string, "API") != 0, "Expected string NOT to match at index 3.");
	free(string);
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_str(list, 15, &string);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_STR_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_STR_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_str(null_list, 0, &string);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_STR_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_STR_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	char **null_string_ptr = NULL;
	result = plist_get_str(list, 0, null_string_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_STR_NULL_INPUT_OUT_VALUE, "Expected pvars_errno == FAILURE_PLIST_GET_STR_NULL_INPUT_OUT_VALUE at index 6.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* ------------------------ */
/* Test 10: plist_get_int() */
/* ------------------------ */
int test_plist_get_int(void)
{
	plist_t *list = plist_create(1);
	
	plist_add_int(list, 0);
	plist_add_int(list, 12);
	plist_add_str(list, "libpvars");
	plist_add_int(list, 1234);
	
	int value = 1;
	bool result;
	
	/* Index 0 */
	result = plist_get_int(list, 0, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(value == 0, "Expected value to be 0 at index 0.");
	
	/* Index 1 */
	result = plist_get_int(list, 1, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(value == 12, "Expected value to be 12 at index 1.");
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_int(list, 2, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno != SUCCESS, "Expected pvars_errno != SUCCESS at index 2.");
	
	/* Index 3 */
	result = plist_get_int(list, 3, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(value == 1234, "Expected value to be 1234 at index 3.");
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_int(list, 15, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_int(null_list, 0, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_INT_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_INT_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	int *null_int_ptr = NULL;
	result = plist_get_int(list, 0, null_int_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_INT_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_INT_NULL_INPUT at index 6.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* ------------------------- */
/* Test 11: plist_get_long() */
/* ------------------------- */
int test_plist_get_long(void)
{
	plist_t *list = plist_create(1);
	
	plist_add_long(list, 0);
	plist_add_long(list, 12);
	plist_add_str(list, "libpvars");
	plist_add_long(list, 1234);
	
	long value = 1;
	bool result;
	
	/* Index 0 */
	result = plist_get_long(list, 0, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(value == 0, "Expected value to be 0 at index 0.");
	
	/* Index 1 */
	result = plist_get_long(list, 1, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(value == 12, "Expected value to be 12 at index 1.");
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_long(list, 2, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno != SUCCESS, "Expected pvars_errno != SUCCESS at index 2.");
	
	/* Index 3 */
	result = plist_get_long(list, 3, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(value == 1234, "Expected value to be 1234 at index 3.");
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_long(list, 15, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LONG_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_LONG_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_long(null_list, 0, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LONG_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_LONG_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	long *null_long_ptr = NULL;
	result = plist_get_long(list, 0, null_long_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LONG_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_LONG_NULL_INPUT at index 6.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* --------------------------- */
/* Test 12: plist_get_double() */
/* --------------------------- */
int test_plist_get_double(void)
{
	plist_t *list = plist_create(1);
	
	plist_add_double(list, 0.7);
	plist_add_double(list, 12.2);
	plist_add_str(list, "libpvars");
	plist_add_double(list, 1234.4);
	
	double value = 1.5;
	bool result;
	
	/* Index 0 */
	result = plist_get_double(list, 0, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(fabs(value - 0.7) < (DBL_EPSILON * fmax(fabs(value), fabs(0.7))), "Expected value to be 0.7 at index 0.");
	
	/* Index 1 */
	result = plist_get_double(list, 1, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(fabs(value - 12.2) < (DBL_EPSILON * fmax(fabs(value), fabs(12.2))), "Expected value to be 12.4 at index 1.");
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_double(list, 2, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DOUBLE_WRONG_TYPE, "Expected pvars_errno == FAILURE_PLIST_GET_DOUBLE_WRONG_TYPE at index 2.");
	
	/* Index 3 */
	result = plist_get_double(list, 3, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(fabs(value - 1234.4) < (DBL_EPSILON * fmax(fabs(value), fabs(1234.4))), "Expected value to be 1234.4 at index 3.");
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_double(list, 15, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DOUBLE_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_DOUBLE_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_double(null_list, 0, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DOUBLE_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_DOUBLE_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	double *null_double_ptr = NULL;
	result = plist_get_double(list, 0, null_double_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DOUBLE_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_DOUBLE_NULL_INPUT at index 6.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* --------------------------- */
/* Test 13: plist_get_float() */
/* --------------------------- */
int test_plist_get_float(void)
{
	plist_t *list = plist_create(1);
	
	plist_add_float(list, 0.7);
	plist_add_float(list, 12.2);
	plist_add_str(list, "libpvars");
	plist_add_float(list, 1234.4);
	
	float value = 1.5;
	bool result;
	
	/* Index 0 */
	result = plist_get_float(list, 0, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(fabsf(value - 0.7f) < FLT_EPSILON, "Expected value to be 0.7 at index 0.");
	
	/* Index 1 */
	result = plist_get_float(list, 1, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(fabsf(value - 12.2f) < FLT_EPSILON, "Expected value to be 12.4 at index 1.");
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_float(list, 2, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_FLOAT_WRONG_TYPE, "Expected pvars_errno == FAILURE_PLIST_GET_FLOAT_WRONG_TYPE at index 2.");
	
	/* Index 3 */
	result = plist_get_float(list, 3, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(fabsf(value - 1234.4f) < FLT_EPSILON, "Expected value to be 1234.4 at index 3.");
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_float(list, 15, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_FLOAT_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_FLOAT_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_float(null_list, 0, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_FLOAT_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_FLOAT_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	float *null_float_ptr = NULL;
	result = plist_get_float(list, 0, null_float_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_FLOAT_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_FLOAT_NULL_INPUT at index 6.");
	
	plist_destroy(list);
	
	TEST_END();
}

/* ------------------------- */
/* Test 14: plist_get_list() */
/* ------------------------- */
int test_plist_get_list(void)
{
	plist_t *list = plist_create(1);
	plist_t *list_child1 = plist_create(1);
	plist_t *list_child2 = plist_create(1);
	plist_t *list_child3 = plist_create(1);
	
	plist_add_str(list_child1, "libpvars");
	plist_add_str(list_child1, "1.0.0.0");
	plist_add_int(list_child2, 12);
	plist_add_long(list_child2, 64123);
	plist_add_float(list_child3, 12.5);
	plist_add_double(list_child3, 65.645);
	
	plist_add_list(list, list_child1);
	plist_add_list(list, list_child2);
	plist_add_int(list, 16);
	plist_add_list(list, list_child3);
	
	plist_t *value = NULL;
	bool result;
	
	/* Index 0 */
	result = plist_get_list(list, 0, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(value != NULL, "Failed to get value at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 0.");
	plist_destroy(value);
	
	/* Index 1 */
	result = plist_get_list(list, 1, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(value != NULL, "Failed to get value at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 1.");
	plist_destroy(value);
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_list(list, 2, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LIST_WRONG_TYPE, "Expected pvars_errno == FAILURE_PLIST_GET_LIST_WRONG_TYPE at index 2.");
	
	/* Index 3 */
	result = plist_get_list(list, 3, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(value != NULL, "Failed to get string at index 3.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 3.");
	plist_destroy(value);
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_list(list, 15, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LIST_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_LIST_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_list(null_list, 0, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LIST_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_LIST_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	plist_t **null_list_ptr = NULL;
	result = plist_get_list(list, 0, null_list_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_LIST_NULL_INPUT_OUT_VALUE, "Expected pvars_errno == FAILURE_PLIST_GET_LIST_NULL_INPUT_OUT_VALUE at index 6.");
	
	/* Index 7 */
	/* Deep copy check after other lists destroyed */
	plist_get_list(list, 0, &value);
	plist_destroy(list);
	plist_destroy(list_child1);
	plist_destroy(list_child2);
	plist_destroy(list_child3);
	char *extracted_from_value = NULL;
	result = plist_get_str(value, 0, &extracted_from_value);
	ASSERT_TRUE(result == true, "Expected result == true at index 7.");
	ASSERT_TRUE(value != NULL, "Expected result != NULL at index 7.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 6.");
	
	plist_destroy(value);
	free(extracted_from_value);
	
	TEST_END();
}

/* ------------------------- */
/* Test 15: plist_get_dict() */
/* ------------------------- */
int test_plist_get_dict(void)
{
	plist_t *list = plist_create(1);
	
	pdict_t *dict1 = pdict_create(1);
	pdict_t *dict2 = pdict_create(1);
	pdict_t *dict3 = pdict_create(1);
	
	pdict_add_int(dict1, "libpvars", 12);
	pdict_add_str(dict1, "1.0.0.0", "latest version");
	pdict_add_int(dict2, "API", 12);
	pdict_add_long(dict2, "library", 64123);
	pdict_add_float(dict3, "code", 12.5);
	pdict_add_double(dict3, "developement", 65.645);
	
	plist_add_dict(list, dict1);
	plist_add_dict(list, dict2);
	plist_add_int(list, 16);
	plist_add_dict(list, dict3);
	
	pdict_t *value = NULL;
	bool result;
	
	/* Index 0 */
	result = plist_get_dict(list, 0, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 0.");
	ASSERT_TRUE(value != NULL, "Failed to get value at index 0.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 0.");
	pdict_destroy(value);
	
	/* Index 1 */
	result = plist_get_dict(list, 1, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 1.");
	ASSERT_TRUE(value != NULL, "Failed to get value at index 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 1.");
	pdict_destroy(value);
	
	/* Index 2 */
	/* Wrong type check */
	result = plist_get_dict(list, 2, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 2.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DICT_WRONG_TYPE, "Expected pvars_errno == FAILURE_PLIST_GET_DICT_WRONG_TYPE at index 2.");
	
	/* Index 3 */
	result = plist_get_dict(list, 3, &value);
	ASSERT_TRUE(result == true, "Expected result == true at index 3.");
	ASSERT_TRUE(value != NULL, "Failed to get string at index 3.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 3.");
	pdict_destroy(value);
	
	/* Index 4 */
	/* Out of bounds check */
	result = plist_get_dict(list, 15, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 4.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DICT_OUT_OF_BOUNDS, "Expected pvars_errno == FAILURE_PLIST_GET_DICT_OUT_OF_BOUNDS at index 4.");
	
	/* Index 5 */
	/* NULL list check  */
	plist_t *null_list = NULL;
	result = plist_get_dict(null_list, 0, &value);
	ASSERT_TRUE(result == false, "Expected result == false at index 5.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DICT_NULL_INPUT, "Expected pvars_errno == FAILURE_PLIST_GET_DICT_NULL_INPUT at index 5.");
	
	/* Index 6 */
	/* NULL out_value check  */
	pdict_t **null_dict_ptr = NULL;
	result = plist_get_dict(list, 0, null_dict_ptr);
	ASSERT_TRUE(result == false, "Expected result == false at index 6.");
	ASSERT_TRUE(pvars_errno == FAILURE_PLIST_GET_DICT_NULL_INPUT_OUT_VALUE, "Expected pvars_errno == FAILURE_PLIST_GET_DICT_NULL_INPUT_OUT_VALUE at index 6.");
	
	/* Index 7 */
	/* Deep copy check after other lists destroyed */
	plist_get_dict(list, 0, &value);
	plist_destroy(list);
	pdict_destroy(dict1);
	pdict_destroy(dict2);
	pdict_destroy(dict3);
	int extracted_from_value;
	result = pdict_get_int(value, "libpvars", &extracted_from_value);
	ASSERT_TRUE(result == true, "Expected result == true at index 7.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "Expected pvars_errno == SUCCESS at index 6.");
	
	pdict_destroy(value);
	
	TEST_END();
}



/* ------------------------- */
/* --- Test Suite Runner --- */
/* ------------------------- */

typedef int (*test_func)();

struct {
	const char *name;
	test_func func;
} test_suite[] = {
	{"test_plist_create", test_plist_create},
	{"test_plist_add_str", test_plist_add_str},
	{"test_plist_add_int", test_plist_add_int},
	{"test_plist_add_long", test_plist_add_long},
	{"test_plist_add_double", test_plist_add_double},
	{"test_plist_add_float", test_plist_add_float},
	{"test_plist_add_list", test_plist_add_list},
	{"test_plist_add_dict", test_plist_add_dict},
	{"test_plist_get_str", test_plist_get_str},
	{"test_plist_get_int", test_plist_get_int},
	{"test_plist_get_long", test_plist_get_long},
	{"test_plist_get_double", test_plist_get_double},
	{"test_plist_get_float", test_plist_get_float},
	{"test_plist_get_list", test_plist_get_list},
	{"test_plist_get_dict", test_plist_get_dict},
	{NULL, NULL}
};

/* ------------ */
/* --- Main --- */
/* ------------ */
int main() {
	int total_tests = 0;
	int failed_tests = 0;

	printf("Running libpvars Test Suite...\n");

	for (int i = 0; test_suite[i].name != NULL; ++i) {
		total_tests++;
		printf("  TEST: %s... ", test_suite[i].name);

		if (test_suite[i].func() != 0) {
			failed_tests++;
			printf("[**FAILED**]\n");
		} else {
			printf("[PASS]\n");
		}
	}

	printf("\n--- Summary ---\n");
	printf("Total Tests Run: %d\n", total_tests);
	printf("Passed: %d\n", total_tests - failed_tests);
	printf("Failed: %d\n", failed_tests);

	return failed_tests == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
