#ifndef PDICT_H
#define PDICT_H

#define STRING_MATCH 0

typedef struct pdict_t pdict_t;

pdict_t *pdict_create(long int initial_capacity);
void pdict_destroy(pdict_t *dict);
void pdict_empty(pdict_t *dict);

void pdict_add_str(pdict_t *dict, char *key, const char *value);
void pdict_add_int(pdict_t *dict, char *key, int value);
void pdict_add_double(pdict_t *dict, char *key, double value);
void pdict_add_long(pdict_t *dict, char *key, long value);
void pdict_add_float(pdict_t *dict, char *key, float value);
void pdict_add_list(pdict_t *dict, char *key, plist_t *value);

#endif
