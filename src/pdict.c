#define _POSIX_C_SOURCE 200809L

#include<stddef.h>
#include<stdlib.h>
#include<string.h>

#include"pvars.h"
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
 * @brief Frees the dynamically allocated data inside a pvar_t struct.
 * * This is crucial for correctly updating a dictionary entry's value or
 * * for cleanup during dictionary destruction.
 * @param pvar_t to be destroyed
 */
static void pvar_destroy_internal(pvar_t *pvar)
{
	if (pvar == NULL || pvar->type == PVAR_TYPE_NONE) {
		return;
	}

	switch (pvar->type) {
		case PVAR_TYPE_STRING:
			if (pvar->data.s != NULL) {
				free(pvar->data.s);
				pvar->data.s = NULL;
			}
			break;
		case PVAR_TYPE_LIST:
			if (pvar->data.ls != NULL) {
				plist_destroy(pvar->data.ls);
				pvar->data.ls = NULL;
			}
			break;
		case PVAR_TYPE_DICT:
			if (pvar->data.dt != NULL) {
				pdict_destroy(pvar->data.dt);
				pvar->data.dt = NULL;
			}
			break;
		case PVAR_TYPE_INT:
		case PVAR_TYPE_DOUBLE:
		case PVAR_TYPE_LONG:
		case PVAR_TYPE_FLOAT:
		case PVAR_TYPE_NONE:
		default:
			break;
	}

	pvar->type = PVAR_TYPE_NONE;
	pvar->data.s = NULL;
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
 * @brief Adds a string to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_str(pdict_t *dict, char *key, const char *value)
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
			pvar_destroy_internal(&(current->value));

			char *new_string = strdup(value);
			if (new_string == NULL) {
				pvars_errno = FAILURE_PDICT_ADD_STR_VALUE_STRDUP_FAILED;
				return;
			}

			current->value.type = PVAR_TYPE_STRING;
			current->value.data.s = new_string;
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
    
	// (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}


/**
 * @brief Adds an int to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_int(pdict_t *dict, char *key, int value)
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
			pvar_destroy_internal(&(current->value));

			current->value.type = PVAR_TYPE_INT;
			current->value.data.i = value;
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
    
	// G. (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}

/**
 * @brief Adds a double to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_double(pdict_t *dict, char *key, double value)
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
			pvar_destroy_internal(&(current->value));

			current->value.type = PVAR_TYPE_DOUBLE;
			current->value.data.d = value;
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
    
	// G. (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}

/**
 * @brief Adds a long to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_long(pdict_t *dict, char *key, long value)
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
			pvar_destroy_internal(&(current->value));

			current->value.type = PVAR_TYPE_LONG;
			current->value.data.l = value;
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
    
	// G. (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}

/**
 * @brief Adds a float to a pdict_t variable
 *
 * @param The address of a dict.
 * @param Char key
 * @param The value to add to the dict
 * @return void
 */
void pdict_add_float(pdict_t *dict, char *key, float value)
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
			pvar_destroy_internal(&(current->value));

			current->value.type = PVAR_TYPE_FLOAT;
			current->value.data.f = value;
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
void pdict_add_list(pdict_t *dict, char *key, plist_t *value)
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
			pvar_destroy_internal(&(current->value));

			plist_t *new_list = plist_copy(value);
			if (new_list == NULL) {
				pvars_errno = FAILURE_PDICT_ADD_LIST_VALUE_PLIST_COPY_FAILED;
				return;
			}

			current->value.type = PVAR_TYPE_LIST;
			current->value.data.ls = new_list;
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

	char *new_list = plist_copy(value);
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
    
	// (Future) Check Load Factor and Resize
	// You would place your load factor check and resize function call here.
}
