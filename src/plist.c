#define _POSIX_C_SOURCE 200809L

#include<stdlib.h>
#include<string.h>

#include"pvars.h"
#include"perrno.h"
#include"pvars_internal.h"
#include"plist_internal.h"

/**
 * @brief Creates and initializes a new plist_t structure.
 *
 * Allocates memory for the list structure and its initial elements array.
 * The elements array is initialized using calloc, ensuring all pvar_t
 * structs have their type set to PVAR_TYPE_NONE (0) and their data union
 * fields (including the string pointer) are NULL.
 *
 * @param initial_capacity The starting capacity for the list. Must be >= 1.
 * @return A pointer to the newly created plist_t structure, or NULL on failure.
 */
plist_t *plist_create(long int initial_capacity)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (initial_capacity < 1) {
		pvars_errno = FAILURE_PLIST_CREATE_CAPACITY_OUT_OF_BOUNDS;
		return NULL;
	}

	plist_t *new_list = malloc(sizeof(plist_t));
	if (new_list == NULL) {
		pvars_errno = FAILURE_PLIST_CREATE_NEW_LIST_MALLOC_FAILED;
		return NULL;
	}

	// Use calloc for pvar_t structs: initializes type to PVAR_TYPE_NONE (0) and data union to zero (NULL pointer)
	new_list->elements = calloc((size_t)initial_capacity, sizeof(pvar_t));
	if (new_list->elements == NULL) {
		free(new_list);
		pvars_errno = FAILURE_PLIST_CREATE_NEW_LIST_DATA_MALLOC_FAILED;
		return NULL;
	}
	
	new_list->capacity = (size_t)initial_capacity;
	new_list->count = 0;

	return new_list;
}

/**
 * @brief Deep copy a list.
 *
 * @param list of elements
 */
plist_t *plist_copy(const plist_t *src)
{
	/* pvars_errno must be clear on each call to determine if pvar_copy fails recursively. See pvar_copy */
	pvars_errno = PERRNO_CLEAR;
	if (src == NULL) {
		pvars_errno = FAILURE_PLIST_COPY_NULL_INPUT;
		return NULL;
	}
	
	plist_t *new_list = plist_create(src->capacity);
	
	if (new_list == NULL) {
		pvars_errno = FAILURE_PLIST_COPY_PLIST_CREATE_FAILED;
		return NULL;
	}
	
	new_list->count = src->count;
	
	for (size_t i = 0; i < src->count; i++) {
		pvar_t new_var = pvar_copy(&src->elements[i]);
		if (!(pvars_errno == SUCCESS)) {
			pvars_errno = FAILURE_PLIST_COPY_PVAR_COPY_FAILED;
			plist_destroy(new_list);
			return NULL;
		}
		new_list->elements[i] = new_var;
	}
	
	pvars_errno = SUCCESS;
	return new_list;
	
}


/**
 * @brief Ensures there is capacity for one more element, resizing if necessary.
 * * @param list The list to check.
 * @return True if capacity is available/resized successfully, false otherwise.
 */
static bool plist_ensure_capacity(plist_t *list)
{
	if (list->count < list->capacity) {
		pvars_errno = SUCCESS;
		return true;
	}

	size_t old_capacity = list->capacity;
	size_t new_capacity = list->capacity * 2;
	
	// Reallocate pvar_t array
	pvar_t *new_elements = (pvar_t *)realloc(list->elements, new_capacity * sizeof(pvar_t));

	if (new_elements == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_REALLOC_FAILED; 
		return false;
	}
	
	list->elements = new_elements;

	// Initialize new memory slots to PVAR_TYPE_NONE (zeroing the entire struct is safe)
	// We use list->elements + old_capacity to get the start of the new block
	memset(list->elements + old_capacity, 0, (new_capacity - old_capacity) * sizeof(pvar_t));

	list->capacity = new_capacity;
	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Removes the element at a given index, shifting subsequent elements.
 *
 * Frees the data of the removed element, shifts the remaining elements down,
 * and decrements the count.
 *
 * @param list The list to modify.
 * @param index The index of the element to remove.
 */
void plist_remove(plist_t *list, size_t index)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_REMOVE_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_REMOVE_OUT_OF_BOUNDS;
		return;
	}

	pvar_destroy_internal(&list->elements[index]);

	// Shift all subsequent elements down
	for (size_t i = index; i < list->count - 1; i++) {
		list->elements[i] = list->elements[i+1];
	}

	memset(&list->elements[list->count - 1], 0, sizeof(pvar_t));
	
	list->count--;
	pvars_errno = SUCCESS;
}

/**
 * @brief Adds a single string to the list.
 *
 * The string is duplicated using strdup and stored in a new pvar_t element.
 *
 * @param list The list to add to.
 * @param value The string to add (will be duplicated).
 */
void plist_add_str(plist_t *list, const char *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_STR_NULL_INPUT_LIST;
		return;
	}

	if (value == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_STR_NULL_STRING_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code
		return;
	}

	// Store the string
	char *new_str = strdup(value);

	if (new_str == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_STR_STRDUP_FAILED;
		return;
	}

	// Populate the pvar_t struct fields
	list->elements[list->count].data.s = new_str;
	list->elements[list->count].type = PVAR_TYPE_STRING;

	list->count++;
}

/**
 * @brief Adds a single integer value to the list.
 *
 * @param list The list to add to.
 * @param value The integer value to store.
 */
void plist_add_int(plist_t *list, int value)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_INT_NULL_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code (FAILURE_PLIST_ADD_REALLOC_FAILED)
		return;
	}
	
	// Data stored directly in the union (no heap allocation needed)
	list->elements[list->count].data.i = value;
	list->elements[list->count].type = PVAR_TYPE_INT;

	list->count++;
}

/**
 * @brief Adds a single double value to the list.
 *
 * @param list The list to add to.
 * @param value The double value to store.
 */
void plist_add_double(plist_t *list, double value)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_DOUBLE_NULL_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code (FAILURE_PLIST_ADD_REALLOC_FAILED)
		return;
	}
	
	// Data stored directly in the union (no heap allocation needed)
	list->elements[list->count].data.d = value;
	list->elements[list->count].type = PVAR_TYPE_DOUBLE;

	list->count++;
}

/**
 * @brief Adds a single long value to the list.
 *
 * @param list The list to add to.
 * @param value The long value to store.
 */
void plist_add_long(plist_t *list, long value)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_LONG_NULL_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code (FAILURE_PLIST_ADD_REALLOC_FAILED)
		return;
	}
	
	// Data stored directly in the union (no heap allocation needed)
	list->elements[list->count].data.l = value;
	list->elements[list->count].type = PVAR_TYPE_LONG;

	list->count++;
}

/**
 * @brief Adds a single float value to the list.
 *
 * @param list The list to add to.
 * @param value The float value to store.
 */
void plist_add_float(plist_t *list, float value)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_FLOAT_NULL_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code (FAILURE_PLIST_ADD_REALLOC_FAILED)
		return;
	}
	
	// Data stored directly in the union (no heap allocation needed)
	list->elements[list->count].data.f = value;
	list->elements[list->count].type = PVAR_TYPE_FLOAT;

	list->count++;
}

/**
 * @brief Adds a single list to the list.
 *
 * The list is stored in a new pvar_t element.
 *
 * @param list The list to add to.
 * @param value The list to add.
 */
void plist_add_list(plist_t *list, const plist_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_LIST_NULL_INPUT;
		return;
	}

	if (value == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_LIST_NULL_LIST_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code
		return;
	}

	plist_t *new_list = plist_copy(value);
	if (new_list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_LIST_PLIST_COPY_FAILED;
		return;
	}
	
	// Populate the pvar_t struct fields
	list->elements[list->count].data.ls = new_list;
	list->elements[list->count].type = PVAR_TYPE_LIST;

	list->count++;
}

/**
 * @brief Adds a single dict to the list.
 *
 * The dict is duplicated using pdict_copy and stored in a new pvar_t element.
 *
 * @param list The list to add to.
 * @param value The dict to add (will be duplicated).
 */
void plist_add_dict(plist_t *list, const pdict_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_DICT_NULL_INPUT;
		return;
	}

	if (value == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_DICT_NULL_DICT_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		// plist_ensure_capacity sets the error code
		return;
	}

	pdict_t *new_dict = pdict_copy(value);

	if (new_dict == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_DICT_PDICT_COPY_FAILED;
		return;
	}

	list->elements[list->count].data.dt = new_dict;
	list->elements[list->count].type = PVAR_TYPE_DICT;

	list->count++;
}

/**
 * @brief Adds a single pvar to the list.
 *
 * The dict is duplicated using pvar_copy and stored in a new pvar_t element.
 *
 * @param list The list to add to.
 * @param value The pvar to add (will be duplicated).
 */
void plist_add_pvar(plist_t *list, const pvar_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_PVAR_NULL_INPUT;
		return;
	}

	if (value == NULL) {
		pvars_errno = FAILURE_PLIST_ADD_PVAR_NULL_PVAR_INPUT;
		return;
	}

	/* Resize capacity if needed */
	if (!plist_ensure_capacity(list)) {
		pvars_errno = FAILURE_PLIST_ADD_PVAR_PLIST_ENSURE_CAPACITY_FAILED;
		return;
	}

	pvar_t new_pvar = pvar_copy(value);

	if (pvars_errno != SUCCESS) {
		pvars_errno = FAILURE_PLIST_ADD_PVAR_PVAR_COPY_FAILED;
		return;
	}

	list->elements[list->count] = new_pvar;

	list->count++;
}

/**
 * @brief Clears the list, freeing memory for all contained elements (like strings).
 *
 * Resets the element count to zero but preserves the allocated capacity.
 *
 * @param list The list to empty.
 */
void plist_empty(plist_t *list)
{
	if (list == NULL || list->elements == NULL) {
		return;
	}

	for (size_t i = 0; i < list->count; i++) {
		pvar_destroy_internal(&list->elements[i]);
	}

	/* Reset count to 0 */
	list->count = 0;
}

/**
 * @brief Destroys the list, freeing all associated memory.
 *
 * Calls plist_empty() to free element data, then frees the elements array,
 * and finally frees the list structure itself.
 *
 * @param list The list to destroy.
 */
void plist_destroy(plist_t *list)
{
	if (list == NULL) {
		pvars_errno = SUCCESS; 
		return;
	}

	plist_empty(list);

	if (list->elements != NULL) {
		free(list->elements);
	}

	free(list);
	pvars_errno = SUCCESS;
}

/**
 * @brief A wrapper function for plist_print_internal to ensure proper formatting
 *
 * @param list The list to print.
 */
void plist_print(const plist_t *list)
{
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_PRINT_NULL_INPUT_LIST;
		return;
	}
	
	plist_print_internal(list);
	putchar('\n');
}

/**
 * @brief Prints the contents of the list to standard output.
 *
 * @param list The list to print.
 */
void plist_print_internal(const plist_t *list)
{

	if (list->elements == NULL) {
		pvars_errno = FAILURE_PLIST_PRINT_INTERNAL_NULL_INPUT_LIST_DATA;
		return;
	}

	pvars_errno = PERRNO_CLEAR; // Clear error before proceeding

	putchar('[');

	for (size_t i = 0; i < list->count; i++) {
		if (i > 0) {
			printf(", ");
		}
		
		pvar_t current = list->elements[i];

		switch (current.type) {
			case PVAR_TYPE_STRING:
				printf("\'%s\'", current.data.s);
				break;
			case PVAR_TYPE_INT:
				printf("%d", current.data.i);
				break;
			case PVAR_TYPE_DOUBLE:
				printf("%lf", current.data.d);
				break;
			case PVAR_TYPE_LONG:
				printf("%ld", current.data.l);
				break;
			case PVAR_TYPE_FLOAT:
				printf("%f", current.data.f);
				break;
			case PVAR_TYPE_LIST:
				/* Recursively print the internal list */
				plist_print_internal(current.data.ls);
				break;
			case PVAR_TYPE_DICT:
				pdict_print_internal(current.data.dt);
				break;
			case PVAR_TYPE_NONE:
				printf("NULL"); 
				break;
			default:
				printf("[Type: Unknown]");
				break;
		}
	}
	putchar(']');
}

/**
 * @brief Returns the current number of elements in the list.
 *
 * @param list The list to query.
 * @return The number of elements (list->count) if the list is valid, 
 * or 0 if the list is NULL.
 */
size_t plist_get_size(const plist_t *list)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		return 0;
	}
	
	return list->count;
}

/**
 * @brief Returns the current capacity of the list.
 *
 * @param list The list to query.
 * @return The capacity (list->capacity) if the list is valid, 
 * or 0 if the list is NULL.
 */
size_t plist_get_capacity(const plist_t *list)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		return 0;
	}
	
	return list->capacity;
}

/**
 * @brief Returns the type of variable of an element in the list stored at the given index.
 *
 * @param list The list to query.
 * @return pvar_type enum (PVAR_TYPE_NONE, PVAR_TYPE_STRING, etc) if the list is valid, 
 * or 0 if the list is NULL, or the index is out of bounds.
 */
pvar_type plist_get_type(const plist_t *list, size_t index)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_GET_TYPE_NULL_INPUT;
		return PVAR_TYPE_NONE;
	}
	
	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_TYPE_OUT_OF_BOUNDS;
		return PVAR_TYPE_NONE;
	}
	
	return list->elements[index].type;
}

/**
 * @brief Retrieves the string value at a given index.
 *
 * Performs boundary checks and, critically, checks if the element's type is
 * PVAR_TYPE_STRING.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @return The string pointer, or NULL if the index is out of bounds or the type is incorrect.
 */
bool plist_get_str(const plist_t *list, size_t index, char **out_value)
{
	pvars_errno = SUCCESS;
	
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_GET_STR_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_STR_OUT_OF_BOUNDS;
		return false;
	}
	
	if (out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_STR_NULL_INPUT_OUT_VALUE;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	if (element->type != PVAR_TYPE_STRING) {
		pvars_errno = FAILURE_PLIST_GET_STR_WRONG_TYPE;
		return false;
	}
	
	*out_value = strdup(element->data.s);
	if (*out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_STR_STRDUP_FAILED;
		return false;
	}
	
	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Retrieves the integer value at a given index.
 *
 * Performs boundary checks and checks if the element's type is PVAR_TYPE_INT.
 * The retrieved value is written to the pointer `out_value`.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @param out_value Pointer where the retrieved integer value should be stored.
 * @return True on success, False on failure (with pvars_errno set).
 */
bool plist_get_int(const plist_t *list, size_t index, int *out_value)
{
	if (list == NULL || out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_INT_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_INT_OUT_OF_BOUNDS;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	// Check type before accessing data.i
	if (element->type != PVAR_TYPE_INT) {
		pvars_errno = FAILURE_PLIST_GET_INT_WRONG_TYPE;
		return false;
	}
	
	*out_value = element->data.i;

	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Retrieves the double value at a given index.
 *
 * Performs boundary checks and checks if the element's type is PVAR_TYPE_DOUBLE.
 * The retrieved value is written to the pointer `out_value`.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @param out_value Pointer where the retrieved double value should be stored.
 * @return True on success, False on failure (with pvars_errno set).
 */
bool plist_get_double(const plist_t *list, size_t index, double *out_value)
{
	if (list == NULL || out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_DOUBLE_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_DOUBLE_OUT_OF_BOUNDS;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	// Check type before accessing data.d
	if (element->type != PVAR_TYPE_DOUBLE) {
		pvars_errno = FAILURE_PLIST_GET_DOUBLE_WRONG_TYPE;
		return false;
	}
	
	*out_value = element->data.d;

	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Retrieves the long value at a given index.
 *
 * Performs boundary checks and checks if the element's type is PVAR_TYPE_LONG.
 * The retrieved value is written to the pointer `out_value`.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @param out_value Pointer where the retrieved long value should be stored.
 * @return True on success, False on failure (with pvars_errno set).
 */
bool plist_get_long(const plist_t *list, size_t index, long *out_value)
{
	if (list == NULL || out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_LONG_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_LONG_OUT_OF_BOUNDS;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	// Check type before accessing data.i
	if (element->type != PVAR_TYPE_LONG) {
		pvars_errno = FAILURE_PLIST_GET_LONG_WRONG_TYPE;
		return false;
	}
	
	*out_value = element->data.l;

	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Retrieves the float value at a given index.
 *
 * Performs boundary checks and checks if the element's type is PVAR_TYPE_FLOAT.
 * The retrieved value is written to the pointer `out_value`.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @param out_value Pointer where the retrieved float value should be stored.
 * @return True on success, False on failure (with pvars_errno set).
 */
bool plist_get_float(const plist_t *list, size_t index, float *out_value)
{
	if (list == NULL || out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_FLOAT_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_FLOAT_OUT_OF_BOUNDS;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	// Check type before accessing data.d
	if (element->type != PVAR_TYPE_FLOAT) {
		pvars_errno = FAILURE_PLIST_GET_FLOAT_WRONG_TYPE;
		return false;
	}
	
	*out_value = element->data.f;

	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Retrieves the list value at a given index.
 *
 * Performs boundary checks and checks if the element's type is PVAR_TYPE_LIST.
 * The retrieved value is written to the pointer `out_value`.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @param out_value Pointer where the retrieved list value should be stored.
 * @return True on success, False on failure (with pvars_errno set).
 */
bool plist_get_list(const plist_t *list, size_t index, plist_t **out_value)
{
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_GET_LIST_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_LIST_OUT_OF_BOUNDS;
		return false;
	}
	
	if (out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_LIST_NULL_INPUT_OUT_VALUE;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	if (element->type != PVAR_TYPE_LIST) {
		pvars_errno = FAILURE_PLIST_GET_LIST_WRONG_TYPE;
		return false;
	}
	
	*out_value = plist_copy(element->data.ls);
	if (*out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_LIST_PLIST_COPY_FAILED;
		return false;
	}
	
	pvars_errno = SUCCESS;
	return true;
}

/**
 * @brief Retrieves the dict value at a given index.
 *
 * Performs boundary checks and checks if the element's type is PVAR_TYPE_DICT.
 * The retrieved value is written to the pointer `out_value`.
 *
 * @param list The list to read from.
 * @param index The index of the element to retrieve.
 * @param out_value Pointer where the retrieved dict value should be stored.
 * @return True on success, False on failure (with pvars_errno set).
 */
bool plist_get_dict(const plist_t *list, size_t index, pdict_t **out_value)
{
	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_GET_DICT_NULL_INPUT;
		return false;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_GET_DICT_OUT_OF_BOUNDS;
		return false;
	}
	
	if (out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_DICT_NULL_INPUT_OUT_VALUE;
		return false;
	}
	
	pvar_t *element = &list->elements[index];

	if (element->type != PVAR_TYPE_DICT) {
		pvars_errno = FAILURE_PLIST_GET_DICT_WRONG_TYPE;
		return false;
	}
	
	*out_value = pdict_copy(element->data.dt);
	if (*out_value == NULL) {
		pvars_errno = FAILURE_PLIST_GET_DICT_PDICT_COPY_FAILED;
		return false;
	}
	
	pvars_errno = SUCCESS;
	return true;
}



/**
 * @brief Sets the string value at a given index.
 *
 * The existing content of the element is freed first (if it was a string).
 * A copy of the new string is stored, and the element's type is set to STRING.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_string The new string to store (will be duplicated).
 */
void plist_set_str(plist_t *list, size_t index, const char *new_string)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_STR_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_STR_OUT_OF_BOUNDS;
		return;
	}

	if (new_string == NULL) {
		pvars_errno = FAILURE_PLIST_SET_STR_NULL_STRING_INPUT;
		return;
	}
	
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	char *current_str = strdup(new_string);
	if (current_str == NULL) {
		pvars_errno = FAILURE_PLIST_SET_STR_STRDUP_FAILED;
		return;
	}

	element->data.s = current_str;
	element->type = PVAR_TYPE_STRING;
}

/**
 * @brief Sets the integer value at a given index.
 *
 * The existing content of the element is freed first (if it was a string).
 * The new integer value is stored, and the element's type is set to INT.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_value The new integer value to store.
 */
void plist_set_int(plist_t *list, size_t index, int new_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_INT_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_INT_OUT_OF_BOUNDS;
		return;
	}
	
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	element->data.i = new_value;
	element->type = PVAR_TYPE_INT;
}

/**
 * @brief Sets the double value at a given index.
 *
 * The existing content of the element is freed first (if it was a string).
 * The new integer value is stored, and the element's type is set to DOUBLE.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_value The new double value to store.
 */
void plist_set_double(plist_t *list, size_t index, double new_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_DOUBLE_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_DOUBLE_OUT_OF_BOUNDS;
		return;
	}
	
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	element->data.d = new_value;
	element->type = PVAR_TYPE_DOUBLE;
}

/**
 * @brief Sets the long value at a given index.
 *
 * The existing content of the element is freed first (if it was a string).
 * The new long value is stored, and the element's type is set to LONG.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_value The new long value to store.
 */
void plist_set_long(plist_t *list, size_t index, long new_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_LONG_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_LONG_OUT_OF_BOUNDS;
		return;
	}
	
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	element->data.l = new_value;
	element->type = PVAR_TYPE_LONG;
}

/**
 * @brief Sets the float value at a given index.
 *
 * The existing content of the element is freed first (if it was a string).
 * The new float value is stored, and the element's type is set to float.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_value The new float value to store.
 */
void plist_set_float(plist_t *list, size_t index, float new_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_FLOAT_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_FLOAT_OUT_OF_BOUNDS;
		return;
	}
	
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	element->data.f = new_value;
	element->type = PVAR_TYPE_FLOAT;
}

/**
 * @brief Sets the list value at a given index.
 *
 * The existing content of the element is freed first (if it was a string or list).
 * A copy of the new list is stored, and the element's type is set to LIST.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_list The new list to store.
 */
void plist_set_list(plist_t *list, size_t index, const plist_t *new_list)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_LIST_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_LIST_OUT_OF_BOUNDS;
		return;
	}

	if (new_list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_LIST_NULL_LIST_INPUT;
		return;
	}

	plist_t *deep_list = plist_copy(new_list);
	if (deep_list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_LIST_PLIST_COPY_FAILED;
		return;
	}
		
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	element->data.ls = deep_list;
	element->type = PVAR_TYPE_LIST;
}

/**
 * @brief Sets the dict value at a given index.
 *
 * The existing content of the element is freed first.
 * A copy of the new dict is stored, and the element's type is set to DICT.
 *
 * @param list The list to modify.
 * @param index The index of the element to set.
 * @param new_dict The new dict to store (will be duplicated).
 */
void plist_set_dict(plist_t *list, size_t index, const pdict_t *new_dict)
{
	pvars_errno = PERRNO_CLEAR;

	if (list == NULL) {
		pvars_errno = FAILURE_PLIST_SET_DICT_NULL_INPUT;
		return;
	}

	if (index >= list->count) {
		pvars_errno = FAILURE_PLIST_SET_DICT_OUT_OF_BOUNDS;
		return;
	}

	if (new_dict == NULL) {
		pvars_errno = FAILURE_PLIST_SET_DICT_NULL_DICT_INPUT;
		return;
	}
	
	pvar_t *element = &list->elements[index];

	pvar_destroy_internal(element);

	pdict_t *current_dict = pdict_copy(new_dict);
	if (current_dict == NULL) {
		pvars_errno = FAILURE_PLIST_SET_DICT_PDICT_COPY_FAILED;
		return;
	}

	element->data.dt = current_dict;
	element->type = PVAR_TYPE_DICT;
}


/**
 * @brief Checks for item in list.
 *
 * @param list of elements
 * @param element to find in list
 */
bool plist_contains(const plist_t *list, pvar_t *element_to_find)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (list == NULL || element_to_find == NULL) {
		pvars_errno = FAILURE_PLIST_CONTAINS_NULL_INPUT;
		return false;
	}

	for (size_t i = 0; i < list->count; i++) {
		/* Return true if pvar_equals returns true */
		if (pvar_equals(&list->elements[i], element_to_find)) {
			return true;
		}
	}
	
	return false;
}
