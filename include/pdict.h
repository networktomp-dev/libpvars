#ifndef PDICT_H
#define PDICT_H

#define STRING_MATCH 0

typedef struct pdict_t pdict_t;

/* --- Public API Function Prototypes --- */

/* plist_t setup and packdown*/
pdict_t *pdict_create(long int initial_capacity);
void pdict_destroy(pdict_t *dict);
void pdict_empty(pdict_t *dict);

void pdict_remove(pdict_t *dict, const char *key);

/* Functions that act on plist_t variables */
void pdict_print(pdict_t *dict);

/* plist_t meta data accessors */
size_t pdict_get_size(pdict_t *dict);
size_t pdict_get_capacity(pdict_t *dict);

/* Core Add element functions (SINGLE ITEM ONLY) */
void pdict_add_str(pdict_t *dict, const char *key, const char *value);
void pdict_add_int(pdict_t *dict, const char *key, int value);
void pdict_add_double(pdict_t *dict, const char *key, double value);
void pdict_add_long(pdict_t *dict, const char *key, long value);
void pdict_add_float(pdict_t *dict, const char *key, float value);
void pdict_add_list(pdict_t *dict, const char *key, plist_t *value);

/* String accessors */
bool pdict_get_str(pdict_t *dict, const char *key, char **out_value);

/* List accessors */
bool pdict_get_list(pdict_t *dict, const char *key, plist_t **out_value);

/* Int accessor */
bool pdict_get_int(pdict_t *dict, const char *key, int *out_value);

#endif
