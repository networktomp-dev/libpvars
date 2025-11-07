#include"perrno.h"

__thread int pvars_errno = 0;

/**
 * @brief Returns the human-readable string associated with the current
 * pvars_errno value.
 *
 * @return A constant string describing the error.
 */
const char *perror_message(void)
{
	switch (pvars_errno) {
		case SUCCESS:
			return "SUCCESS: Operation was successful";
			
		/* plist_create Failures */
		case FAILURE_PLIST_CREATE_CAPACITY_OUT_OF_BOUNDS:
			return "FAILURE: Function plist_create() cannot have a capacity less than 1";
		case FAILURE_PLIST_CREATE_NEW_LIST_MALLOC_FAILED:
			return "FAILURE: malloc() failed to allocate memory to plist_t *new_list in function plist_create()";
		case FAILURE_PLIST_CREATE_NEW_LIST_DATA_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_list->elements in function plist_create()";
		
		/* Core List Manipulation Failures */
		case FAILURE_PLIST_ADD_NULL_INPUT_LIST:
			return "FAILURE: Null list passed to function plist_add_*()";
		case FAILURE_PLIST_ADD_REALLOC_FAILED:
			return "FAILURE: realloc() failed to allocated memory when resizing list->elements in list add/set function";
		
		/* plist_add_str / plist_get_str / plist_set_str Failures */
		case FAILURE_PLIST_ADD_NULL_STRING_INPUT:
			return "FAILURE: NULL string passed to function plist_add_str()";
		case FAILURE_PLIST_ADD_STRDUP_FAILED:
			return "FAILURE: strdup() failed in function plist_add_str()";
		case FAILURE_PLIST_GET_STR_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_get_str()";
		case FAILURE_PLIST_GET_STR_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_get_str()";
		case FAILURE_PLIST_GET_STR_WRONG_TYPE:
			return "FAILURE: Cannot retrieve data: Element is not of the expected type (expected string) in function plist_get_str()";
		case FAILURE_PLIST_SET_STR_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_set_str()";
		case FAILURE_PLIST_SET_STR_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_set_str()";
		case FAILURE_PLIST_SET_STR_NULL_STRING_INPUT:
			return "FAILURE: NULL string input passed to function plist_set_str()";
		case FAILURE_PLIST_SET_STR_STRDUP_FAILED:
			return "FAILURE: Function strdup() failed to duplicate string in function plist_set_str()";
			
		/* plist_add_int / plist_get_int / plist_set_int Failures */
		case FAILURE_PLIST_ADD_INT_NULL_INPUT:
			return "FAILURE: NULL input list passed to function plist_add_int()";
		case FAILURE_PLIST_GET_INT_NULL_INPUT:
			return "FAILURE: NULL list or NULL output pointer passed to function plist_get_int()";
		case FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_get_int()";
		case FAILURE_PLIST_GET_INT_WRONG_TYPE:
			return "FAILURE: Cannot retrieve data: Element is not of the expected type (expected int) in function plist_get_int()";
		case FAILURE_PLIST_SET_INT_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_set_int()";
		case FAILURE_PLIST_SET_INT_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_set_int()";

		/* plist_add_double / plist_get_double / plist_set_double Failures */
		case FAILURE_PLIST_ADD_DOUBLE_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_add_double()";
		case FAILURE_PLIST_GET_DOUBLE_NULL_INPUT:
			return "FAILURE: NULL list or NULL output pointer passed to function plist_get_double()";
		case FAILURE_PLIST_GET_DOUBLE_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_get_double()";
		case FAILURE_PLIST_GET_DOUBLE_WRONG_TYPE:
			return "FAILURE: Cannot retrieve data: Element is not of the expected type (expected double) in function plist_get_double()";
		case FAILURE_PLIST_SET_DOUBLE_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_set_double()";
		case FAILURE_PLIST_SET_DOUBLE_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_set_double()";
			
		/* plist_add_long / plist_get_long / plist_set_long Failures */
		case FAILURE_PLIST_ADD_LONG_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_add_long()";
		case FAILURE_PLIST_GET_LONG_NULL_INPUT:
			return "FAILURE: NULL list or NULL output pointer passed to function plist_get_long()";
		case FAILURE_PLIST_GET_LONG_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_get_long()";
		case FAILURE_PLIST_GET_LONG_WRONG_TYPE:
			return "FAILURE: Cannot retrieve data: Element is not of the expected type (expected long) in function plist_get_long()";
		case FAILURE_PLIST_SET_LONG_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_set_long()";
		case FAILURE_PLIST_SET_LONG_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_set_long()";

		/* plist_add_float / plist_get_float / plist_set_float Failures */
		case FAILURE_PLIST_ADD_FLOAT_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_add_float()";
		case FAILURE_PLIST_GET_FLOAT_NULL_INPUT:
			return "FAILURE: NULL list or NULL output pointer passed to function plist_get_float()";
		case FAILURE_PLIST_GET_FLOAT_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_get_float()";
		case FAILURE_PLIST_GET_FLOAT_WRONG_TYPE:
			return "FAILURE: Cannot retrieve data: Element is not of the expected type (expected float) in function plist_get_float()";
		case FAILURE_PLIST_SET_FLOAT_NULL_INPUT:
			return "FAILURE: NULL list passed to function plist_set_float()";
		case FAILURE_PLIST_SET_FLOAT_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_set_float()";
			
		/* plist_add_list / plist_get_list / plist_set_list Failures */
		case FAILURE_PLIST_ADD_LIST_NULL_INPUT:
			return "FAILURE: NULL carrier list passed to function plist_add_list()";
		case FAILURE_PLIST_ADD_LIST_NULL_LIST_INPUT:
			return "FAILURE: NULL list passed to function plist_add_list()";
		case FAILURE_PLIST_ADD_LIST_PLIST_COPY_FAILED:
			return "FAILURE: plist_copy() failed in function plist_add_list()";
		case FAILURE_PLIST_GET_LIST_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_get_list()";
		case FAILURE_PLIST_GET_LIST_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_get_list()";
		case FAILURE_PLIST_GET_LIST_WRONG_TYPE:
			return "FAILURE: Cannot retrieve data: Element is not of the expected type (expected plist_t) in function plist_get_list()";
		case FAILURE_PLIST_SET_LIST_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_set_list()";
		case FAILURE_PLIST_SET_LIST_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_set_list()";
		case FAILURE_PLIST_SET_LIST_NULL_LIST_INPUT:
			return "FAILURE: NULL list input passed to function plist_set_list()";
		case FAILURE_PLIST_SET_LIST_PLIST_COPY_FAILED:
			return "FAILURE: plist_copy() failed in function plist_set_list()";

		/* plist_contains Failures */
		case FAILURE_PLIST_CONTAINS_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_contains()";
			
		/* pvar_equals / pvar_copy Failures */
		case FAILURE_PVAR_EQUALS_NULL_INPUT:
			return "FAILURE: NULL input passed to function pvar_equals()";
		case FAILURE_PVAR_EQUALS_UNKNOWN_VAR_TYPE:
			return "FAILURE: Cannot compare variable: Variable type unknown! in function pvar_equals()";
		case FAILURE_PVAR_COPY_NULL_INPUT:
			return "FAILURE: NULL input passed to function pvar_copy()";
		case FAILURE_PVAR_COPY_STRDUP_FAILED:
			return "FAILURE: strdup() failed in function pvar_copy()";
		case FAILURE_PVAR_COPY_PLIST_COPY_FAILED:
			return "FAILURE: plist_copy() failed in function pvar_copy()";
		
		/* plist_copy Failures */
		case FAILURE_PLIST_COPY_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_copy()";
		case FAILURE_PLIST_COPY_PLIST_CREATE_FAILED:
			return "FAILURE: plist_create() failed in function plist_copy()";
		case FAILURE_PLIST_COPY_PVAR_COPY_FAILED:
			return "FAILURE: pvar_copy() failed in function plist_copy()";
			
		/* General Failures */
		case FAILURE_PLIST_PRINT_NULL_INPUT_LIST:
			return "FAILURE: NULL list passed to function plist_print()";
		case FAILURE_PLIST_PRINT_INTERNAL_NULL_INPUT_LIST_DATA:
			return "FAILURE: list->elements is NULL in function plist_print_internal()";
		case FAILURE_PLIST_REMOVE_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_remove()";
		case FAILURE_PLIST_REMOVE_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_remove()";
			
		/* pdict_create Failures */
		case FAILURE_PDICT_CREATE_CAPACITY_OUT_OF_BOUNDS:
			return "FAILURE: Function pdict_create() cannot have a capacity less than 1";
		case FAILURE_PDICT_CREATE_NEW_DICT_MALLOC_FAILED:
			return "FAILURE: malloc() failed to allocate memory to pdict_t *new_dict in function pdict_create()";
		case FAILURE_PDICT_CREATE_NEW_DICT_BUCKETS_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_dict->buckets in function pdict_create()";
			
		/* pdict_add_str Failures */
		case FAILURE_PDICT_ADD_STR_NULL_INPUT_DICT:
			return "FAILURE: NULL dict input passed to function pdict_add_str()";
		case FAILURE_PDICT_ADD_STR_NULL_INPUT_KEY:
			return "FAILURE: NULL key input passed to function pdict_add_str()";
		case FAILURE_PDICT_ADD_STR_NULL_INPUT_VALUE:
			return "FAILURE: NULL value input passed to function pdict_add_str()";
		case FAILURE_PDICT_ADD_STR_VALUE_STRDUP_FAILED:
			return "FAILURE: strdup() failed to allocate memory to new_string in function pdict_add_str()";
		case FAILURE_PDICT_ADD_STR_ENTRY_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_entry in function pdict_add_str()";
		case FAILURE_PDICT_ADD_STR_KEY_STRDUP_FAILED:
			return "FAILURE: strdup() failed to allocate memory to new_entry->key in function pdict_add_str()";
		
		/* pdict_add_int Failures */
		case FAILURE_PDICT_ADD_INT_NULL_INPUT_DICT:
			return "FAILURE: NULL dict input passed to function pdict_add_int()";
		case FAILURE_PDICT_ADD_INT_NULL_INPUT_KEY:
			return "FAILURE: NULL key input passed to function pdict_add_int()";
		case FAILURE_PDICT_ADD_INT_ENTRY_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_entry in function pdict_add_int()";
		case FAILURE_PDICT_ADD_INT_KEY_STRDUP_FAILED:
			return "FAILURE: strdup() failed to allocate memory to new_entry->key in function pdict_add_int()";
		
		/* pdict_add_double Failures */
		case FAILURE_PDICT_ADD_DOUBLE_NULL_INPUT_DICT:
			return "FAILURE: NULL dict input passed to function pdict_add_double()";
		case FAILURE_PDICT_ADD_DOUBLE_NULL_INPUT_KEY:
			return "FAILURE: NULL key input passed to function pdict_add_double()";
		case FAILURE_PDICT_ADD_DOUBLE_ENTRY_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_entry in function pdict_add_double()";
		case FAILURE_PDICT_ADD_DOUBLE_KEY_STRDUP_FAILED:
			return "FAILURE: strdup() failed to allocate memory to new_entry->key in function pdict_add_double()";
		
		/* pdict_add_long Failures */
		case FAILURE_PDICT_ADD_LONG_NULL_INPUT_DICT:
			return "FAILURE: NULL dict input passed to function pdict_add_long()";
		case FAILURE_PDICT_ADD_LONG_NULL_INPUT_KEY:
			return "FAILURE: NULL key input passed to function pdict_add_long()";
		case FAILURE_PDICT_ADD_LONG_ENTRY_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_entry in function pdict_add_long()";
		case FAILURE_PDICT_ADD_LONG_KEY_STRDUP_FAILED:
			return "FAILURE: strdup() failed to allocate memory to new_entry->key in function pdict_add_long()";
		
		/* pdict_add_float Failures */
		case FAILURE_PDICT_ADD_FLOAT_NULL_INPUT_DICT:
			return "FAILURE: NULL dict input passed to function pdict_add_float()";
		case FAILURE_PDICT_ADD_FLOAT_NULL_INPUT_KEY:
			return "FAILURE: NULL key input passed to function pdict_add_float()";
		case FAILURE_PDICT_ADD_FLOAT_ENTRY_MALLOC_FAILED:
			return "FAILURE: Unable to allocate memory to new_entry in function pdict_add_float()";
		case FAILURE_PDICT_ADD_FLOAT_KEY_STRDUP_FAILED:
			return "FAILURE: strdup() failed to allocate memory to new_entry->key in function pdict_add_float()";

		default:
			return "Unknown error number";
	}
}
