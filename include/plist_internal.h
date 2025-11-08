#ifndef PLIST_INTERNAL_H
#define PLIST_INTERNAL_H

#include<stdbool.h>
#include <stddef.h> /* For size_t */

#include"perrno.h" /* For pvar_type enum */
#include"pvars.h"
#include"pdict_internal.h"

/**
 * @brief The full definition of the list structure.
 * * This definition is hidden from the user and only visible in pvars.c
 * by including this header.
 */
struct plist_t {
	pvar_t *elements; /* Pointer to the dynamic array of pvar_t structs */
	size_t count; /* Number of elements in the list */
	size_t capacity; /* Total allocated space (number of char * slots) */
};


/* Helper Function definitions */
void plist_print_internal(plist_t *list);
bool pvar_equals(pvar_t *a, pvar_t *b);

pvar_t pvar_copy(const pvar_t *src);


#endif /* PLIST_INTERNAL_H */
