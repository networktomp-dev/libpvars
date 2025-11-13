#define _POSIX_C_SOURCE 200809L

#include<stddef.h>
#include<stdlib.h>
#include<string.h>

#include"pvars.h"
#include"pvars_internal.h"
#include"perrno.h"
#include"pdict_internal.h"

/**
 * @brief Creates a hash value from the key and capacity given
 *
 * @param key string
 * @param capacity
 * @return a hash value stored in size_t variable.
 */
size_t pdict_hash(const char *key, size_t capacity)
{
	unsigned long hash = 5381;
	int c;

	// Standard DJB2 loop
	while ((c = *key++)) {
		// hash * 33 + c
		hash = ((hash << 5) + hash) + c; 
	}

	// Modulo operation to fit the hash into the table capacity
	return (size_t)(hash % capacity);
}

/**
 * @brief Creates and initializes a new pdict_t structure.
 *
 * Allocates memory for the dict structure and its initial buckets array.
 * The buckets array is initialized using calloc
 *
 * @param initial_capacity The starting capacity for the dict. Must be >= 1.
 * @return A pointer to the newly created pdict_t structure, or NULL on failure.
 */
pdict_t *pdict_create(long int initial_capacity)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (initial_capacity < 1) {
		pvars_errno = FAILURE_PDICT_CREATE_CAPACITY_OUT_OF_BOUNDS;
		return NULL;
	}

	pdict_t *new_dict = malloc(sizeof(pdict_t));
	if (new_dict == NULL) {
		pvars_errno = FAILURE_PDICT_CREATE_NEW_DICT_MALLOC_FAILED;
		return NULL;
	}

	// Use calloc for pvar_t structs: initializes type to PVAR_TYPE_NONE (0) and data union to zero (NULL pointer)
	new_dict->buckets = calloc((size_t)initial_capacity, sizeof(pdict_entry_t *));
	if (new_dict->buckets == NULL) {
		free(new_dict);
		pvars_errno = FAILURE_PDICT_CREATE_NEW_DICT_BUCKETS_MALLOC_FAILED;
		return NULL;
	}
	
	new_dict->capacity = (size_t)initial_capacity;
	new_dict->count = 0;

	return new_dict;
}

/**
 * @brief Deep copies a pdict_entry_t variable.
 *
 * @param pdict_entry_t
 */
pdict_entry_t *pdict_entry_copy(const pdict_entry_t *src)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (src == NULL) {
		pvars_errno = FAILURE_PDICT_ENTRY_COPY_NULL_INPUT;
		return NULL;
	}
	
	pdict_entry_t *dest = calloc(1, sizeof(pdict_entry_t));
	if (dest == NULL) {
		pvars_errno = FAILURE_PDICT_ENTRY_COPY_NEW_ENTRY_MALLOC_FAILED;
		return NULL;
	}
	
	pvar_t new_pvar = pvar_copy(&src->value);
	if (pvars_errno != SUCCESS) {
		pvars_errno = FAILURE_PDICT_ENTRY_COPY_PVAR_COPY_FAILED;
		free(dest);
		return NULL;
	}
	
	char *key = strdup(src->key);
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ENTRY_COPY_STRDUP_FAILED;
		free(dest);
		return NULL;
	}
	
	dest->value = new_pvar;
	dest->key = key;
	dest->next = NULL; 
	
	pvars_errno = SUCCESS;
	return dest;
}

/**
 * @brief Deep copy a dict.
 *
 * @param dict
 */
pdict_t *pdict_copy(const pdict_t *src)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (src == NULL) {
		pvars_errno = FAILURE_PDICT_COPY_NULL_INPUT;
		return NULL;
	}
	
	pdict_t *new_dict = pdict_create(src->capacity);
	if (new_dict == NULL) {
		pvars_errno = FAILURE_PDICT_COPY_PDICT_CREATE_FAILED;
		return NULL;
	}
	
	for (size_t i = 0; i < src->capacity; i++) {
		pdict_entry_t *current = src->buckets[i];
		pdict_entry_t *new_head = NULL;
		pdict_entry_t *new_tail = NULL;
		
		while (current != NULL) {
			pdict_entry_t *new_entry = pdict_entry_copy(current);
			if (new_entry == NULL) {
				pvars_errno = FAILURE_PDICT_COPY_PDICT_ENTRY_COPY_FAILED;
				pdict_destroy(new_dict);
				return NULL;
			}
			
			if (new_head == NULL) {
				new_head = new_entry;
				new_tail = new_entry;
			} else {
				new_tail->next = new_entry;
				new_tail = new_entry;
			}
			
			current = current->next;
		}
		
		new_dict->buckets[i] = new_head;
	}
	
	new_dict->count = src->count;
	
	pvars_errno = SUCCESS;
	
	return new_dict;
}

/**
 * @brief Empties the dict, but leaves dict and dict->buckets
 * memory intact for future use.
 *
 * @param The dict to be emptied
 * @return void
 */
void pdict_empty(pdict_t *dict)
{
	if (dict == NULL || dict->buckets == NULL) {
		return;
	}
	
	for (size_t i = 0; i < dict->capacity; i++) {
		pdict_entry_t *current = dict->buckets[i];
		pdict_entry_t *next_entry;
		
		while (current != NULL) {
			next_entry = current->next;
			if (current->key != NULL) {
				free(current->key);
			}
			
			pvar_destroy_internal(&(current->value));
			
			free(current);
			current = next_entry;
		}
		
		dict->buckets[i] = NULL;
	}
	
	dict->count = 0;
}

/**
 * @brief Destroys all the memory associated with the dict
 *
 * @param The dict to be destroyed
 * @return void
 */
void pdict_destroy(pdict_t *dict)
{
	if (dict == NULL) {
		return;
	}
	
	pdict_empty(dict);
	
	free(dict->buckets);
	
	free(dict);
}

/**
 * @brief Helper function to pdict_print
 *
 * @param dict_t pointer.
 */
void pdict_print_internal(const pdict_t *dict)
{
	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_PRINT_INTERNAL_NULL_INPUT;
		return;
	}
	
	pvars_errno = PERRNO_CLEAR;

	putchar('[');
	
	for (size_t i = 0; i < dict->capacity; i++) {
		pdict_entry_t *current = dict->buckets[i];
		while (current != NULL) {			
			printf("[Key: \"%s\" | Value: ", current->key);
			
			switch (current->value.type) {
				case PVAR_TYPE_STRING:
					printf("\'%s\']", current->value.data.s);
					break;
				case PVAR_TYPE_LIST:	
					plist_print_internal(current->value.data.ls);
					putchar(']');
					break;
				case PVAR_TYPE_DICT:
					pdict_print_internal(current->value.data.dt);
					putchar(']');
					break;
				case PVAR_TYPE_INT:
					printf("%d]", current->value.data.i);
					break;
				case PVAR_TYPE_DOUBLE:
					printf("%lf]", current->value.data.d);
					break;
				case PVAR_TYPE_LONG:
					printf("%d]", current->value.data.i);
					break;
				case PVAR_TYPE_FLOAT:
					printf("%f]", current->value.data.f);
					break;
				case PVAR_TYPE_NONE:
					printf("[TYPE: NONE]");
					break;
				default:
					printf("[TYPE: UNKNOWN]");
					break;
			}
			
			current = current->next;
		}
	}
	
	putchar(']');
}

/**
 * @brief Prints the full contents of a dict
 *
 * @param dict_t pointer.
 */
void pdict_print(const pdict_t *dict)
{
	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_PRINT_NULL_INPUT;
		return;
	}
	
	pdict_print_internal(dict);
	putchar('\n');
}

/**
 * @brief Returns the current number of elements in the dict.
 *
 * @param dict - The dict to query.
 * @return The number of elements (dict->count) if the dict is valid, 
 * or 0 if the dict is NULL.
 */
size_t pdict_get_size(const pdict_t *dict)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (dict == NULL) {
		return 0;
	}
	
	return dict->count;
}

/**
 * @brief Returns the current capacity of the dict.
 *
 * @param dict - The list to query.
 * @return The capacity (dict->capacity) if the dict is valid, 
 * or 0 if the dict is NULL.
 */
size_t pdict_get_capacity(const pdict_t *dict)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (dict == NULL) {
		return 0;
	}
	
	return dict->capacity;
}

/**
 * @brief Returns the type of variable of a pdict_entry in the dict using the key.
 *
 * @param dict The dict to query.
 * @param key
 * @return pvar_type enum (PVAR_TYPE_NONE, PVAR_TYPE_STRING, etc) if the dict is valid, 
 * or 0 if the dict is NULL, or the key is not found.
 */
pvar_type pdict_get_type(const pdict_t *dict, const char *key)
{
	pvars_errno = PERRNO_CLEAR;
	
	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_TYPE_NULL_INPUT;
		return PVAR_TYPE_NONE;
	}
	
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_TYPE_NULL_KEY_INPUT;
		return PVAR_TYPE_NONE;
	}
	
	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			return current->value.type;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_GET_TYPE_KEY_NOT_FOUND;
	return PVAR_TYPE_NONE;
}
 
/**
 * @brief Searches for the existence of an entry in a dict
 *
 * @param dict The dict to query.
 * @param key
 * @return true if the entry exists, false if not
 */
bool pdict_contains(const pdict_t *dict, const char *key)
{
 	pvars_errno = PERRNO_CLEAR;
	
	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_CONTAINS_NULL_INPUT;
		return false;
	}
	
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_CONTAINS_NULL_KEY_INPUT;
		return false;
	}
	
	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			return true;
		}
		current = current->next;
	}
	
	return false;
}
 
/**
 * @brief Exports all the keys available in a dict. This function is a shallow traversal only. It doesn't dive into inner dictionaries.
 *
 * @param dict The constant dictionary to query.
 * @return A newly allocated plist_t containing PVAR_TYPE_STRING elements (deep copies of the keys). Returns NULL on failure. The caller is responsible for destroying the returned list.
 */
plist_t *pdict_get_keys(const pdict_t * dict)
{
 	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_KEYS_NULL_INPUT;
		return NULL;
	}

	plist_t *new_list = plist_create(dict->capacity);
	if (new_list == NULL) {
		pvars_errno = FAILURE_PDICT_GET_KEYS_PLIST_CREATE_FAILED;
		return NULL;
	}
	
	for (size_t i = 0; i < dict->capacity; i++) {
		pdict_entry_t *current = dict->buckets[i];
		while (current != NULL) {
			plist_add_str(new_list, current->key);
			if (pvars_errno != SUCCESS) {
				pvars_errno = FAILURE_PDICT_GET_KEYS_PLIST_ADD_STR_FAILED; // Intentional double handling of pvars_errno.
				plist_destroy(new_list);
				return NULL;
			}
			
			current = current->next;
		}
	}

	pvars_errno = SUCCESS;
	return new_list;
}
 
/**
 * @brief Exports all the values available in a dict. This function is a shallow traversal only. It doesn't dive into inner dictionaries.
 *
 * @param dict The constant dictionary to query.
 * @return A newly allocated plist_t containing pvar_t elements (deep copies of the values). Returns NULL on failure. The caller is responsible for destroying the returned list.
 */
plist_t *pdict_get_values(const pdict_t *dict)
{
 	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_VALUES_NULL_INPUT;
		return NULL;
	}

	plist_t *new_list = plist_create(dict->capacity);
	if (new_list == NULL) {
		pvars_errno = FAILURE_PDICT_GET_VALUES_PLIST_CREATE_FAILED;
		return NULL;
	}
	
	for (size_t i = 0; i < dict->capacity; i++) {
		pdict_entry_t *current = dict->buckets[i];
		while (current != NULL) {
			plist_add_pvar(new_list, &(current->value));
			if (pvars_errno != SUCCESS) {
				pvars_errno = FAILURE_PDICT_GET_VALUES_PLIST_ADD_PVAR_FAILED;
				plist_destroy(new_list);
				return NULL;
			}
			
			current = current->next;
		}
	}

	pvars_errno = SUCCESS;
	return new_list;
}

/**
 * @brief removes a pvar_t from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @return void
 */
void pdict_remove(pdict_t *dict, const char *key)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_REMOVE_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_REMOVE_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	
	pdict_entry_t *current = dict->buckets[bucket_index];
	pdict_entry_t *previous = NULL;
	
	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (previous == NULL) {
				dict->buckets[bucket_index] = current->next;
			} else {
				previous->next = current->next;
			}
			pvar_destroy_internal(&(current->value));
			free(current->key);
			free(current);
			
			dict->count--;
			return;
		}
		previous = current;
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_REMOVE_KEY_NOT_FOUND;
}

 
/**
 * @brief Adds a string to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_str(pdict_t *dict, const char *key, const char *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_STR_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_STR_NULL_INPUT_KEY;
		return;
	}
	if (value == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_STR_NULL_INPUT_VALUE;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_STR_KEY_EXISTS;
			return;
		}
		current = current->next;
	}



	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_STR_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_STR_KEY_STRDUP_FAILED;
		return;
	}

	char *new_string = strdup(value);
	if (new_string == NULL) {
		free(new_entry->key);
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_STR_VALUE_STRDUP_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_STRING;
	new_entry->value.data.s = new_string;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
}


/**
 * @brief Adds an int to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_int(pdict_t *dict, const char *key, int value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_INT_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_INT_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_INT_KEY_EXISTS;
			return;
		}
		current = current->next;
	}

	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_INT_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_INT_KEY_STRDUP_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_INT;
	new_entry->value.data.i = value;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
}

/**
 * @brief Adds a double to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_double(pdict_t *dict, const char *key, double value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DOUBLE_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DOUBLE_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_DOUBLE_KEY_EXISTS;
			return;
		}
		current = current->next;
	}

	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DOUBLE_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_DOUBLE_KEY_STRDUP_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_DOUBLE;
	new_entry->value.data.d = value;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
}

/**
 * @brief Adds a long to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_long(pdict_t *dict, const char *key, long value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LONG_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LONG_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_LONG_KEY_EXISTS;
			return;
		}
		current = current->next;
	}

	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LONG_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_LONG_KEY_STRDUP_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_LONG;
	new_entry->value.data.l = value;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
}

/**
 * @brief Adds a float to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_float(pdict_t *dict, const char *key, float value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_FLOAT_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_FLOAT_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_FLOAT_KEY_EXISTS;
			return;
		}
		current = current->next;
	}

	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_FLOAT_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_FLOAT_KEY_STRDUP_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_FLOAT;
	new_entry->value.data.f = value;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
    
	// G. (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}

/**
 * @brief Adds a list to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_list(pdict_t *dict, const char *key, const plist_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LIST_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LIST_NULL_INPUT_KEY;
		return;
	}
	if (value == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LIST_NULL_INPUT_VALUE;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_LIST_KEY_EXISTS;
			return;
		}
		current = current->next;
	}



	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_LIST_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_LIST_KEY_STRDUP_FAILED;
		return;
	}

	plist_t *new_list = plist_copy(value);
	if (new_list == NULL) {
		free(new_entry->key);
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_LIST_VALUE_PLIST_COPY_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_LIST;
	new_entry->value.data.ls = new_list;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
}

/**
 * @brief Adds a dict to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_dict(pdict_t *dict, const char *key, const pdict_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DICT_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DICT_NULL_INPUT_KEY;
		return;
	}
	if (value == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DICT_NULL_INPUT_VALUE;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvars_errno = FAILURE_PDICT_ADD_DICT_KEY_EXISTS;
			return;
		}
		current = current->next;
	}



	pdict_entry_t *new_entry = calloc(1, sizeof(pdict_entry_t));
	if (new_entry == NULL) {
		pvars_errno = FAILURE_PDICT_ADD_DICT_ENTRY_MALLOC_FAILED;
		return;
	}

	new_entry->key = strdup(key);
	if (new_entry->key == NULL) {
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_DICT_KEY_STRDUP_FAILED;
		return;
	}

	pdict_t *new_dict = pdict_copy(value);
	if (new_dict == NULL) {
		free(new_entry->key);
		free(new_entry);
		pvars_errno = FAILURE_PDICT_ADD_DICT_VALUE_PDICT_COPY_FAILED;
		return;
	}

	new_entry->value.type = PVAR_TYPE_DICT;
	new_entry->value.data.dt = new_dict;

	new_entry->next = dict->buckets[bucket_index];
	dict->buckets[bucket_index] = new_entry;

	dict->count++;
	
	pvars_errno = SUCCESS;
}

/**
 * @brief Retrieves a string from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_str(pdict_t *dict, const char *key, char **out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_STR_NULL_INPUT_DICT;
		*out_value = NULL;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_STR_NULL_INPUT_KEY;
		*out_value = NULL;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_STR_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_STRING) {
				pvars_errno = FAILURE_PDICT_GET_STR_WRONG_TYPE;
				*out_value = NULL;
				return false;
			}

			*out_value = strdup(current->value.data.s);
			if (*out_value == NULL) {
				pvars_errno = FAILURE_PDICT_GET_STR_STRDUP_FAILED;
				return false;
			}
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_STR_KEY_NOT_FOUND;
	*out_value = NULL;
	return false;
}

/**
 * @brief Retrieves a list from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_list(pdict_t *dict, const char *key, plist_t **out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_LIST_NULL_INPUT_DICT;
		*out_value = NULL;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_LIST_NULL_INPUT_KEY;
		*out_value = NULL;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_LIST_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_LIST) {
				pvars_errno = FAILURE_PDICT_GET_LIST_WRONG_TYPE;
				*out_value = NULL;
				return false;
			}

			*out_value = plist_copy(current->value.data.ls);
			if (*out_value == NULL) {
				pvars_errno = FAILURE_PDICT_GET_LIST_PLIST_COPY_FAILED;
				return false;
			}
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_LIST_KEY_NOT_FOUND;
	*out_value = NULL;
	return false;
}

/**
 * @brief Retrieves a dict from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_dict(pdict_t *dict, const char *key, pdict_t **out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_DICT_NULL_INPUT_DICT;
		*out_value = NULL;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_DICT_NULL_INPUT_KEY;
		*out_value = NULL;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_DICT_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_DICT) {
				pvars_errno = FAILURE_PDICT_GET_DICT_WRONG_TYPE;
				*out_value = NULL;
				return false;
			}

			*out_value = pdict_copy(current->value.data.dt);
			if (*out_value == NULL) {
				pvars_errno = FAILURE_PDICT_GET_DICT_PDICT_COPY_FAILED;
				return false;
			}
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_DICT_KEY_NOT_FOUND;
	*out_value = NULL;
	return false;
}

/**
 * @brief Retrieves an int from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_int(pdict_t *dict, const char *key, int *out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_INT_NULL_INPUT_DICT;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_INT_NULL_INPUT_KEY;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_INT_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_INT) {
				pvars_errno = FAILURE_PDICT_GET_INT_WRONG_TYPE;
				return false;
			}

			*out_value = current->value.data.i;
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_INT_KEY_NOT_FOUND;
	return false;
}

/**
 * @brief Retrieves a double from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_double(pdict_t *dict, const char *key, double *out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_DOUBLE_NULL_INPUT_DICT;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_DOUBLE_NULL_INPUT_KEY;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_DOUBLE_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_DOUBLE) {
				pvars_errno = FAILURE_PDICT_GET_DOUBLE_WRONG_TYPE;
				return false;
			}

			*out_value = current->value.data.d;
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_DOUBLE_KEY_NOT_FOUND;
	return false;
}

/**
 * @brief Retrieves a long from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_long(pdict_t *dict, const char *key, long *out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_LONG_NULL_INPUT_DICT;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_LONG_NULL_INPUT_KEY;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_LONG_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_LONG) {
				pvars_errno = FAILURE_PDICT_GET_LONG_WRONG_TYPE;
				return false;
			}

			*out_value = current->value.data.l;
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_LONG_KEY_NOT_FOUND;
	return false;
}

/**
 * @brief Retrieves a float from a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to store the retrieved value
 * @return bool
 */
bool pdict_get_float(pdict_t *dict, const char *key, float *out_value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_GET_FLOAT_NULL_INPUT_DICT;
		return false;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_GET_FLOAT_NULL_INPUT_KEY;
		return false;
	}
	if (out_value == NULL) {
		pvars_errno = FAILURE_PDICT_GET_FLOAT_NULL_INPUT_OUT_VALUE;
		return false;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			if (current->value.type != PVAR_TYPE_FLOAT) {
				pvars_errno = FAILURE_PDICT_GET_FLOAT_WRONG_TYPE;
				return false;
			}

			*out_value = current->value.data.f;
			
			pvars_errno = SUCCESS;
			return true;
		}
		current = current->next;
	}

	pvars_errno = FAILURE_PDICT_GET_FLOAT_KEY_NOT_FOUND;
	return false;
}

/**
 * @brief Replaces an element with a string in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_str(pdict_t *dict, const char *key, const char *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_STR_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_STR_NULL_INPUT_KEY;
		return;
	}
	if (value == NULL) {
		pvars_errno = FAILURE_PDICT_SET_STR_NULL_INPUT_VALUE;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));

			char *new_string = strdup(value);
			if (new_string == NULL) {
				pvars_errno = FAILURE_PDICT_SET_STR_VALUE_STRDUP_FAILED;
				return;
			}

			current->value.type = PVAR_TYPE_STRING;
			current->value.data.s = new_string;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_STR_VALUE_NOT_FOUND;
    
	// (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}

/**
 * @brief Replaces an element with a list in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_list(pdict_t *dict, const char *key, const plist_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_LIST_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_LIST_NULL_INPUT_KEY;
		return;
	}
	if (value == NULL) {
		pvars_errno = FAILURE_PDICT_SET_LIST_NULL_INPUT_VALUE;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));

			plist_t *new_list = plist_copy(value);
			if (new_list == NULL) {
				pvars_errno = FAILURE_PDICT_SET_LIST_VALUE_PLIST_COPY_FAILED;
				return;
			}

			current->value.type = PVAR_TYPE_LIST;
			current->value.data.ls = new_list;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_LIST_VALUE_NOT_FOUND;
}

/**
 * @brief Replaces an element with a dict in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_dict(pdict_t *dict, const char *key, const pdict_t *value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_DICT_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_DICT_NULL_INPUT_KEY;
		return;
	}
	if (value == NULL) {
		pvars_errno = FAILURE_PDICT_SET_DICT_NULL_INPUT_VALUE;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));

			pdict_t *new_dict = pdict_copy(value);
			if (new_dict == NULL) {
				pvars_errno = FAILURE_PDICT_SET_DICT_VALUE_PDICT_COPY_FAILED;
				return;
			}

			current->value.type = PVAR_TYPE_DICT;
			current->value.data.dt = new_dict;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_DICT_VALUE_NOT_FOUND;
}

/**
 * @brief Replaces an element with an int in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_int(pdict_t *dict, const char *key, int value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_INT_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_INT_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));


			current->value.type = PVAR_TYPE_INT;
			current->value.data.i = value;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_INT_VALUE_NOT_FOUND;
}

/**
 * @brief Replaces an element with an double in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_double(pdict_t *dict, const char *key, double value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_DOUBLE_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_DOUBLE_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));


			current->value.type = PVAR_TYPE_DOUBLE;
			current->value.data.d = value;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_DOUBLE_VALUE_NOT_FOUND;
}

/**
 * @brief Replaces an element with an long in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_long(pdict_t *dict, const char *key, long value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_LONG_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_LONG_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));


			current->value.type = PVAR_TYPE_LONG;
			current->value.data.l = value;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_LONG_VALUE_NOT_FOUND;
}

/**
 * @brief Replaces an element with an float in a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add in to the dict
 * @return void
 */
void pdict_set_float(pdict_t *dict, const char *key, float value)
{
	pvars_errno = PERRNO_CLEAR;

	if (dict == NULL) {
		pvars_errno = FAILURE_PDICT_SET_FLOAT_NULL_INPUT_DICT;
		return;
	}
	if (key == NULL) {
		pvars_errno = FAILURE_PDICT_SET_FLOAT_NULL_INPUT_KEY;
		return;
	}

	size_t bucket_index = pdict_hash(key, dict->capacity);
	pdict_entry_t *current = dict->buckets[bucket_index];

	while (current != NULL) {
		if (strcmp(current->key, key) == STRING_MATCH) {
			pvar_destroy_internal(&(current->value));


			current->value.type = PVAR_TYPE_FLOAT;
			current->value.data.f = value;
			
			pvars_errno = SUCCESS;
			return;
		}
		current = current->next;
	}
	
	pvars_errno = FAILURE_PDICT_SET_FLOAT_VALUE_NOT_FOUND;
}
