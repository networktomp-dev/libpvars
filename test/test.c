#include<stdio.h>

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

size_t tests_run = 0;

/* Start of a test function */
#define TEST_START(name) \
	printf("TEST:\t\t%s... ", name); \
	fflush(stdout); \
	tests_run++;

/* End of a test function */
#define TEST_END() \
	printf("SUCCESS\n"); \
	pvars_errno = PERRNO_CLEAR; \
	return 0;

/* ---------------------- */
/* Test 1: plist_create() */
/* ---------------------- */
int test_plist_create(void)
{
	TEST_START("test_plist_create")
	
	plist_t *list = plist_create(0);
	ASSERT_TRUE(list == NULL, "Did not get the expected NULL.");
	
	/* Create and destroy the same list several times */
	/* Index 0 */
	list = plist_create(1);
	ASSERT_TRUE(list != NULL, "List allocation failed.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0.");
	ASSERT_TRUE(list->capacity == 1, "Initial list capacity should be 1.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 0");
	
	plist_destroy(list);
	
	/* Index 1 */
	list = plist_create(1024);
	ASSERT_TRUE(list != NULL, "List allocation failed.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0.");
	ASSERT_TRUE(list->capacity == 1024, "Initial list capacity should be 1024.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 1");

	plist_destroy(list);
	
	/* Index 2 */
	list = plist_create(9999);
	ASSERT_TRUE(list != NULL, "List allocation failed.");
	ASSERT_TRUE(list->count == 0, "Initial list count should be 0.");
	ASSERT_TRUE(list->capacity == 9999, "Initial list capacity should be 9999.");
	ASSERT_TRUE(pvars_errno == SUCCESS, "pvars_errno expected success at index 2");

	plist_destroy(list);
	
	TEST_END();
}

/* ----------------------- */
/* Test 2: plist_add_str() */
/* ----------------------- */
int test_plist_add_str(void)
{
	TEST_START("test_plist_add_str");
	
	plist_t *list = plist_create(0);
	
	plist_add_str(list, "libpvars");

	plist_destroy(list);
	
	TEST_END();
}

/* ---------------------- */
/* Test 1: plist_copy() */
/* ---------------------- */
int test_plist_copy(void)
{
	TEST_START("test_plist_copy")
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
