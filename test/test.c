#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pvars.h"
#include "plist.h"
#include "pdict.h"
#include "pvars_internal.h"
#include "pdict_internal.h" // ADDED: Required for pdict_entry_t and pdict_hash access

// Global pvars_errno declaration (to link with the one defined in perrno.c)
extern __thread int pvars_errno;

#define TEST_PASS() (printf("	\033[32m[PASS]\033[0m %s\n", __func__))
#define TEST_FAIL(msg) (printf("	\033[31m[FAIL]\033[0m %s: %s\n", __func__, msg))

// Define STRING_MATCH to match standard strcmp behavior
#define STRING_MATCH 0

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

// --- Test Cases for pvar_destroy_internal ---

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

// --- Test Cases for pdict_remove ---

/**
 * @brief Test suite for pdict_remove, focusing on linked list integrity
 * and count updates.
 */
void test_pdict_remove_suite(void)
{
	printf("--- pdict_remove Test Suite ---\n");
	
	// Test Case 1: Remove from an empty dictionary (Should not crash)
	pdict_t *dict_empty = pdict_create(4);
	printf("Running test_pdict_remove_empty_dict...\n");
	pdict_remove(dict_empty, "nonexistent");
	if (dict_empty->count != 0) {
		TEST_FAIL("Empty Dict: Count unexpectedly changed.");
	} else {
		TEST_PASS();
	}
	pdict_destroy(dict_empty);


	// Test Case 2: Remove a non-existent key from a non-empty dictionary
	pdict_t *dict_nonexistent = pdict_create(4);
	pdict_add_int(dict_nonexistent, "A", 1);
	pdict_add_int(dict_nonexistent, "B", 2);
	size_t initial_count = dict_nonexistent->count;
	
	printf("Running test_pdict_remove_nonexistent_key...\n");
	pdict_remove(dict_nonexistent, "Z");
	if (dict_nonexistent->count != initial_count) {
		TEST_FAIL("Nonexistent Key: Count unexpectedly changed.");
	} else {
		TEST_PASS();
	}
	pdict_destroy(dict_nonexistent);
	
	
	// Test Case 3: Simple removal - single item dictionary
	pdict_t *dict_single = pdict_create(4);
	pdict_add_int(dict_single, "key1", 100);
	printf("Running test_pdict_remove_single_item...\n");
	pdict_remove(dict_single, "key1");
	if (dict_single->count != 0 || dict_single->buckets[pdict_hash("key1", dict_single->capacity)] != NULL) {
		TEST_FAIL("Single Item: Did not remove item or update count/bucket.");
	} else {
		TEST_PASS();
	}
	pdict_destroy(dict_single);
	
	
	// Test Case 4: Linked List Removal (Head, Middle, Tail)
	pdict_t *dict_chain = pdict_create(1); // Force all entries into the same bucket (index 0)
	pdict_add_int(dict_chain, "A", 1); // Head
	pdict_add_int(dict_chain, "B", 2); // Middle
	pdict_add_int(dict_chain, "C", 3); // Tail
	
	size_t bucket = pdict_hash("A", dict_chain->capacity); // Should be 0
	
	// Sub-Test 4.1: Remove MIDDLE node ("B")
	printf("Running test_pdict_remove_middle_node...\n");
	pdict_remove(dict_chain, "B");
	pdict_entry_t *head = dict_chain->buckets[bucket];
	if (dict_chain->count != 2 || head == NULL || head->next == NULL || head->next->next != NULL) {
		TEST_FAIL("Middle Node: List integrity failure (expected A->C->NULL).");
	} else if (strcmp(head->key, "C") != STRING_MATCH || strcmp(head->next->key, "A") != STRING_MATCH) {
		TEST_FAIL("Middle Node: Wrong nodes remaining.");
	} else {
		TEST_PASS();
	}
	
	// Sub-Test 4.1b: Check remaining key/value integrity after MIDDLE removal
	printf("Running test_pdict_remove_middle_integrity...\n");
	// dict_chain currently has C->A->NULL (after 'B' removal in 4.1, insertion order is reversed with pdict)
	// head is "C", head->next is "A"
	if (head->next->value.type != PVAR_TYPE_INT || head->next->value.data.i != 1) {
		TEST_FAIL("Middle Integrity: Remaining node 'A' value corrupted.");
	} else if (head->value.type != PVAR_TYPE_INT || head->value.data.i != 3) {
		TEST_FAIL("Middle Integrity: Remaining node 'C' value corrupted.");
	} else {
		TEST_PASS();
	}
	
	// Sub-Test 4.2: Remove HEAD node ("A")
	printf("Running test_pdict_remove_head_node...\n");
	pdict_remove(dict_chain, "A");
	head = dict_chain->buckets[bucket];
	if (dict_chain->count != 1 || head == NULL || head->next != NULL) {
		TEST_FAIL("Head Node: List integrity failure (expected C->NULL).");
	} else if (strcmp(head->key, "C") != STRING_MATCH) {
		TEST_FAIL("Head Node: Wrong node remaining.");
	} else {
		TEST_PASS();
	}
	
	// Sub-Test 4.3: Remove TAIL node ("C")
	printf("Running test_pdict_remove_tail_node...\n");
	pdict_remove(dict_chain, "C");
	head = dict_chain->buckets[bucket];
	if (dict_chain->count != 0 || head != NULL) {
		TEST_FAIL("Tail Node: List integrity failure (expected NULL).");
	} else {
		TEST_PASS();
	}
	
	pdict_destroy(dict_chain);

	printf("--- Test Suite Complete ---\n");
	
	// Test Case 5: Multiple Middle Removals
	pdict_t *dict_multi = pdict_create(1); 
	pdict_add_int(dict_multi, "X", 10); // Tail
	pdict_add_int(dict_multi, "Y", 20); // Middle 1
	pdict_add_int(dict_multi, "Z", 30); // Middle 2
	pdict_add_int(dict_multi, "W", 40); // Head
	size_t multi_bucket = pdict_hash("X", dict_multi->capacity); 
	pdict_entry_t *multi_head;

	printf("Running test_pdict_remove_multi_middle_1...\n");
	// Remove "Z" (second middle node) -> Expected: W->Y->X->NULL
	pdict_remove(dict_multi, "Z");
	multi_head = dict_multi->buckets[multi_bucket];
	if (dict_multi->count != 3 || multi_head == NULL || multi_head->next == NULL || multi_head->next->next == NULL || multi_head->next->next->next != NULL) {
		TEST_FAIL("Multi Middle 1: List length or termination failure.");
	} else if (strcmp(multi_head->key, "W") != STRING_MATCH || strcmp(multi_head->next->key, "Y") != STRING_MATCH || strcmp(multi_head->next->next->key, "X") != STRING_MATCH) {
		TEST_FAIL("Multi Middle 1: Wrong nodes or order remaining (Expected W, Y, X).");
	} else {
		TEST_PASS();
	}

	printf("Running test_pdict_remove_multi_middle_2...\n");
	// Remove "Y" (now the middle node) -> Expected: W->X->NULL
	pdict_remove(dict_multi, "Y");
	multi_head = dict_multi->buckets[multi_bucket];
	if (dict_multi->count != 2 || multi_head == NULL || multi_head->next == NULL || multi_head->next->next != NULL) {
		TEST_FAIL("Multi Middle 2: List length or termination failure.");
	} else if (strcmp(multi_head->key, "W") != STRING_MATCH || strcmp(multi_head->next->key, "X") != STRING_MATCH) {
		TEST_FAIL("Multi Middle 2: Wrong nodes or order remaining (Expected W, X).");
	} else {
		TEST_PASS();
	}

	pdict_destroy(dict_multi);
}


// --- Main Test Runner ---

int main(void) {
	// Run pvar_destroy_internal tests
	printf("\n--- pvar_destroy_internal Test Suite ---\n");
	
	test_pvar_destroy_internal_string();
	test_pvar_destroy_internal_list();
	test_pvar_destroy_internal_dict();
	test_pvar_destroy_internal_edge_cases();
	
	// Run pdict_remove tests
	test_pdict_remove_suite();
	
	printf("\n--- All Test Suites Complete ---\n");
	
	return 0;
}
