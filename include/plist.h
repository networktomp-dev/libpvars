#ifndef PLIST_H
#define PLIST_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h> /* Essential for bool and true/false */
#include<stddef.h>  /* Essential for size_t */

#include"pvars.h"
#include"perrno.h"

typedef struct plist_t plist_t;

/* --- Public API Function Prototypes --- */

/* plist_t setup and packdown*/
plist_t *plist_create(long int initial_capacity);				// Test 1
plist_t *plist_copy(const plist_t *src);					// Test 24

/* Cleanup functions */
void plist_empty(plist_t *list);						// Test 24
void plist_destroy(plist_t *list);

/* Functions that act on plist_t variables */
void plist_print(const plist_t *list);

/* plist_t meta data accessors */
size_t plist_get_size(const plist_t *list);					// Test 23
size_t plist_get_capacity(const plist_t *list);					// Test 23
pvar_type plist_get_type(const plist_t *list, size_t index);			// Test 23

/* Core Add element functions (SINGLE ITEM ONLY) */
void plist_add_str(plist_t *list, const char *value);				// Test 2
void plist_add_int(plist_t *list, int value);					// Test 3
void plist_add_double(plist_t *list, double value);				// Test 5
void plist_add_long(plist_t *list, long value);					// Test 4
void plist_add_float(plist_t *list, float value);				// Test 6
void plist_add_list(plist_t *list, const plist_t *value);			// Test 7
void plist_add_dict(plist_t *list, const pdict_t *value);			// Test 8
void plist_add_pvar(plist_t *list, const pvar_t *value);

/* String accessors */
bool plist_get_str(const plist_t *list, size_t index, char **out_value);	// Test 9
void plist_set_str(plist_t *list, size_t index, const char *new_string);	// Test 16

/* Integer accessors */
bool plist_get_int(const plist_t *list, size_t index, int *out_value);		// Test 10
void plist_set_int(plist_t *list, size_t index, int new_value);			// Test 17

/* Double accessors */
bool plist_get_double(const plist_t *list, size_t index, double *out_value);	// Test 12
void plist_set_double(plist_t *list, size_t index, double new_value);		// Test 19

/* Long accessors */
bool plist_get_long(const plist_t *list, size_t index, long *out_value);	// Test 11
void plist_set_long(plist_t *list, size_t index, long new_value);		// Test 18

/* Float accessors */
bool plist_get_float(const plist_t *list, size_t index, float *out_value);	// Test 13
void plist_set_float(plist_t *list, size_t index, float new_value);		// Test 20

/* List accessors */
bool plist_get_list(const plist_t *list, size_t index, plist_t **out_value);	// Test 14
void plist_set_list(plist_t *list, size_t index, const plist_t *new_list);	// Test 21

/* Dict accessors */
bool plist_get_dict(const plist_t *list, size_t index, pdict_t **out_value);	// Test 15
void plist_set_dict(plist_t *list, size_t index, const pdict_t *new_dict);	// Test 22

/* Remove element */
void plist_remove(plist_t *list, size_t index);					// Test 23

/* Functions that query list */
bool plist_contains(const plist_t *list, pvar_t *element_to_find);		// Test 25


#endif /* PLIST_H */
