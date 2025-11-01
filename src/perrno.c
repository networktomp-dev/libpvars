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
			return "FAILURE: malloc() failed to allocate memory to plist_t *new list in function plist_create()";
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

		/* plist_contains Failures */
		case FAILURE_PLIST_CONTAINS_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_contains()";
			
		/* pvar_equals Failures */
		case FAILURE_PVAR_EQUALS_NULL_INPUT:
			return "FAILURE: NULL variable passed to function pvar_equals()";
		case FAILURE_PVAR_EQUALS_UNKNOWN_VAR_TYPE:
			return "FAILURE: Cannot compare variable: Variable type unknown! in function pvar_equals()";
			
		/* General Failures */
		case FAILURE_PLIST_PRINT_NULL_INPUT_LIST:
			return "FAILURE: NULL list passed to function plist_print()";
		case FAILURE_PLIST_PRINT_NULL_INPUT_LIST_DATA:
			return "FAILURE: list->elements is NULL in function plist_print()";
		case FAILURE_PLIST_REMOVE_NULL_INPUT:
			return "FAILURE: NULL input passed to function plist_remove()";
		case FAILURE_PLIST_REMOVE_OUT_OF_BOUNDS:
			return "FAILURE: Passed index is out of bounds in function plist_remove()";
			
		default:
			return "Unknown error number";
	}
}
