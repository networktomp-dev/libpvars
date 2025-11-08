#define _POSIX_C_SOURCE 200809L

#include"pvars.h"
#include"pvars_internal.h"

/**
 * @brief Frees the dynamically allocated data inside a pvar_t struct.
 * * This is crucial for correctly updating a list or dictionary entry's value or
 * * for cleanup during a list or dictionary's destruction.
 * @param pvar_t to be destroyed
 */
void pvar_destroy_internal(pvar_t *pvar)
{
	if (pvar == NULL || pvar->type == PVAR_TYPE_NONE) {
		return;
	}

	switch (pvar->type) {
		case PVAR_TYPE_STRING:
			if (pvar->data.s != NULL) {
				free(pvar->data.s);
				pvar->data.s = NULL;
			}
			break;
		case PVAR_TYPE_LIST:
			if (pvar->data.ls != NULL) {
				plist_destroy(pvar->data.ls);
				pvar->data.ls = NULL;
			}
			break;
		case PVAR_TYPE_DICT:
			if (pvar->data.dt != NULL) {
				pdict_destroy(pvar->data.dt);
				pvar->data.dt = NULL;
			}
			break;
		case PVAR_TYPE_INT:
		case PVAR_TYPE_DOUBLE:
		case PVAR_TYPE_LONG:
		case PVAR_TYPE_FLOAT:
		case PVAR_TYPE_NONE:
		default:
			break;
	}

	pvar->type = PVAR_TYPE_NONE;
}
