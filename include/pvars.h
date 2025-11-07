#ifndef PVARS_H
#define PVARS_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief OPAQUE DATA TYPE: plist_t is a forward declaration.
 * * The full structure definition is hidden in pvars.c (via plist_internal.h).
 * This prevents users from directly modifying internal members like 'count' or 
 * 'capacity', enforcing the use of the public API functions.
 */
typedef struct plist_t plist_t;
typedef struct pdict_t pdict_t;

/* Enum to track the type of data stored */
typedef enum {
	PVAR_TYPE_NONE = 0, /* For an empty or uninitialised slot (value is 0 for calloc safety) */
	PVAR_TYPE_STRING,
	PVAR_TYPE_INT,
	PVAR_TYPE_DOUBLE,
	PVAR_TYPE_LONG,
	PVAR_TYPE_FLOAT,
	PVAR_TYPE_LIST,
	PVAR_TYPE_DICT
	/* Future types to go here */
} pvar_type;

/* Union to hold the actual value, shared across different types */
typedef union {
	char *s;
	int i;
	double d;
	long l;
	float f;
	plist_t *ls;
	pdict_t *dt;
	/* Future types to go here */
} pvar_data;

/* Structure combining the type and the data (the new list element) */
typedef struct {
	pvar_type type;
	pvar_data data;
} pvar_t;

#include"plist.h"
#include"pdict.h"

#endif /* PVARS_H */
