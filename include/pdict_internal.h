#ifndef PDICT_INTERNAL_H
#define PDICT_INTERNAL_H

#include"pvars.h"
#include"plist_internal.h"

/**
 * @brief Represents a single key-value pair in the dictionary.
 * The key is always a dynamically allocated string (char *).
 * The value is a pvar_t, allowing it to hold any type.
 */
typedef struct pdict_entry_t {
	char *key;           // The dictionary key (dynamically allocated string)
	pvar_t value;        // The dictionary value (can be any pvar_t type)
	struct pdict_entry_t *next; // Pointer for separate chaining (linked list)
} pdict_entry_t;

/**
 * @brief The full definition of the dictionary structure (Hash Table).
 * This is hidden from the user.
 */
struct pdict_t {
	pdict_entry_t **buckets; // Array of pointers to pdict_entry_t (the buckets)
	size_t count;            // Number of elements currently in the dictionary
	size_t capacity;         // Size of the 'buckets' array (number of slots)
	// You'll need a load factor and resize logic later, but this is enough for now.
};

size_t pdict_hash(const char *key, size_t capacity);
void pdict_print_internal(pdict_t *dict);

#endif
