#ifndef PVARS_INTERNAL_H
#define PVARS_INTERNAL_H

/* Helper functions */
void pvar_destroy_internal(pvar_t *pvar);
bool pvar_equals(pvar_t *a, pvar_t *b);
pvar_t pvar_copy(const pvar_t *src);

#endif
