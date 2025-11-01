#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<stdbool.h> /* Essential for bool and true/false */
#include<stddef.h>  /* Essential for size_t */

#include"perrno.h"

#ifndef PVARS_H
#define PVARS_H

/**
 * @brief OPAQUE DATA TYPE: plist_t is now a forward declaration.
 * * The full structure definition is hidden in pvars.c (via plist_internal.h).
 * This prevents users from directly modifying internal members like 'count' or 
 * 'capacity', enforcing the use of the public API functions.
 */
typedef struct plist_t plist_t;

/* Enum to track the type of data stored */
typedef enum {
	PVAR_TYPE_NONE = 0, /* For an empty or uninitialised slot (value is 0 for calloc safety) */
	PVAR_TYPE_STRING,
	PVAR_TYPE_INT,
	PVAR_TYPE_DOUBLE,
	PVAR_TYPE_LONG,
	PVAR_TYPE_FLOAT
	/* Future types to go here */
} pvar_type;

/* Union to hold the actual value, shared across different types */
typedef union {
	char *s;
	int i;
	double d;
	long l;
	float f;
	/* Future types to go here */
} pvar_data;

/* Structure combining the type and the data (the new list element) */
typedef struct {
	pvar_type type;
	pvar_data data;
} pvar_t;

/* --- Public API Function Prototypes --- */

/* plist_t setup and packdown*/
plist_t *plist_create(long int initial_capacity);

/* Cleanup functions */
void plist_empty(plist_t *list);
void plist_destroy(plist_t *list);

/* Functions that act on plist_t variables */
void plist_print(plist_t *list);

/* plist_t meta data accessors */
size_t plist_get_size(plist_t *list);
size_t plist_get_capacity(plist_t *list);

/* Core Add element functions (SINGLE ITEM ONLY) */
void plist_add_str(plist_t *list, const char *value);
void plist_add_int(plist_t *list, int value);
void plist_add_double(plist_t *list, double value);
void plist_add_long(plist_t *list, long value);
void plist_add_float(plist_t *list, float value);

/* String accessors */
bool plist_get_str(plist_t *list, size_t index, char **out_value);
void plist_set_str(plist_t *list, size_t index, char *new_string);

/* Integer accessors */
bool plist_get_int(plist_t *list, size_t index, int *out_value);
void plist_set_int(plist_t *list, size_t index, int new_value);

/* Double accessors */
bool plist_get_double(plist_t *list, size_t index, double *out_value);
void plist_set_double(plist_t *list, size_t index, double new_value);

/* Long accessors */
bool plist_get_long(plist_t *list, size_t index, long *out_value);
void plist_set_long(plist_t *list, size_t index, long new_value);

/* Float accessors */
bool plist_get_float(plist_t *list, size_t index, float *out_value);
void plist_set_float(plist_t *list, size_t index, float new_value);

/* Remove element */
void plist_remove(plist_t *list, size_t index);

/* Functions that query list */
bool plist_contains(plist_t *list, pvar_t *element_to_find);


#endif /* PVARS_H */
