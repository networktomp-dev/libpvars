#ifndef PDICT_H
#define PDICT_H

#define STRING_MATCH 0

typedef struct pdict_t pdict_t;

/* --- Public API Function Prototypes --- */

/* plist_t setup and packdown*/
pdict_t *pdict_create(long int initial_capacity);
pdict_t *pdict_copy(const pdict_t *src);

/* Cleanup functions */
void pdict_destroy(pdict_t *dict);
void pdict_empty(pdict_t *dict);

void pdict_remove(pdict_t *dict, const char *key);

/* Functions that act on plist_t variables */
void pdict_print(const pdict_t *dict);

/* plist_t meta data accessors */
size_t pdict_get_size(const pdict_t *dict);
size_t pdict_get_capacity(const pdict_t *dict);
pvar_type pdict_get_type(const pdict_t *dict, const char *key);

/* Data Extraction */
bool pdict_contains(const pdict_t *dict, const char *key);
plist_t *pdict_get_keys(const pdict_t * dict);

/* Core Add element functions (SINGLE ITEM ONLY) */
void pdict_add_str(pdict_t *dict, const char *key, const char *value);
void pdict_add_int(pdict_t *dict, const char *key, int value);
void pdict_add_double(pdict_t *dict, const char *key, double value);
void pdict_add_long(pdict_t *dict, const char *key, long value);
void pdict_add_float(pdict_t *dict, const char *key, float value);
void pdict_add_list(pdict_t *dict, const char *key, const plist_t *value);
void pdict_add_dict(pdict_t *dict, const char *key, const pdict_t *value);

/* String accessors */
bool pdict_get_str(pdict_t *dict, const char *key, char **out_value);
void pdict_set_str(pdict_t *list, const char *key, const char *new_string);

/* List accessors */
bool pdict_get_list(pdict_t *dict, const char *key, plist_t **out_value);
void pdict_set_list(pdict_t *dict, const char *key, const plist_t *value);

/* Dict accessors */
bool pdict_get_dict(pdict_t *dict, const char *key, pdict_t **out_value);
void pdict_set_dict(pdict_t *dict, const char *key, const pdict_t *value);

/* Int accessor */
bool pdict_get_int(pdict_t *dict, const char *key, int *out_value);
void pdict_set_int(pdict_t *dict, const char *key, int value);

/* Double accessor */
bool pdict_get_double(pdict_t *dict, const char *key, double *out_value);
void pdict_set_double(pdict_t *dict, const char *key, double value);

/* Long accessor */
bool pdict_get_long(pdict_t *dict, const char *key, long *out_value);
void pdict_set_long(pdict_t *dict, const char *key, long value);

/* Float accessor */
bool pdict_get_float(pdict_t *dict, const char *key, float *out_value);
void pdict_set_float(pdict_t *dict, const char *key, float value);

#endif
