#include<stdio.h>
#include<math.h>
#include<float.h>

#include"pvars.h"
#include"pvars_internal.h" 
#include"plist_internal.h" 
#include"pdict_internal.h" 

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

/* ----------------------- */
/* Test 4: plist_add_long() */
/* ----------------------- */
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

/* ----------------------- */
/* Test 5: plist_add_double() */
/* ----------------------- */
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

/* ----------------------- */
/* Test 6: plist_add_float() */
/* ----------------------- */
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

/* ----------------------- */
/* Test 7: plist_add_list() */
/* ----------------------- */
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

/* ----------------------- */
/* Test 8: plist_add_dict() */
/* ----------------------- */
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

/* ---------------------- */
/* Test 1: plist_copy() */
/* ---------------------- */
int test_plist_copy(void)
{
	plist_t *list = plist_create(0);
	ASSERT_TRUE(list != NULL, "List allocation failed.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0.");

	plist_destroy(list);
	
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
