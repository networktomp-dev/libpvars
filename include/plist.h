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
plist_t *plist_create(long int initial_capacity);
plist_t *plist_copy(const plist_t *src);

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
void plist_add_list(plist_t *list, plist_t *value);

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

/* List accessors */
bool plist_get_list(plist_t *list, size_t index, plist_t **out_value);
void plist_set_list(plist_t *list, size_t index, plist_t *new_list);

/* Remove element */
void plist_remove(plist_t *list, size_t index);

/* Functions that query list */
bool plist_contains(plist_t *list, pvar_t *element_to_find);


#endif /* PLIST_H */
