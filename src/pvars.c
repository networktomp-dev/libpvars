#define _POSIX_C_SOURCE 200809L

#include<math.h>     // For fabs() and fabsf()
#include<float.h>    // For FLT_EPSILON and DBL_EPSILON#include"pvars.h"

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

/**
 * @brief Compares two variables.
 *
 * @param element a to be compared
 * @param element b to be compared
 */
bool pvar_equals(pvar_t *a, pvar_t *b)
{
	/* This check should really be redundant as the caller function must check for NULL input first */
	if (a == NULL || b == NULL) {
		pvars_errno = FAILURE_PVAR_EQUALS_NULL_INPUT;
		return false;
	}
	
	/* The following doesn't trigger an error number because the brief is technically fulfilled. Error is set to SUCCESS */
	if (a->type != b->type) {
		pvars_errno = SUCCESS;
		return false;
	}
	
	switch (a->type) {
		case PVAR_TYPE_STRING:
			/* Extra curly braces creates new scope for the int declaration. Compilers with stricter standars should be satisfied */
			{
				int strcmp_result = strcmp(a->data.s, b->data.s);
				if (strcmp_result != 0) {
					return false;
				}
			}
			return true;
		case PVAR_TYPE_INT:
			return (a->data.i == b->data.i); // Conditional statement evaluates to either true or false
		case PVAR_TYPE_LONG:
			return (a->data.l == b->data.l); // Conditional statement evaluates to either true or false
		case PVAR_TYPE_DOUBLE:
			if (fabs(a->data.d - b->data.d) > DBL_EPSILON) {
				return false;
			}
			return true;
		case PVAR_TYPE_FLOAT:
			// Floats: Check if the absolute difference is within a small epsilon (FLT_EPSILON)
			// fabsf() is the float version of fabs()
			if (fabsf(a->data.f - b->data.f) > FLT_EPSILON) {
				return false;
			}
			return true;
		case PVAR_TYPE_NONE:
			return true;
		default:
			pvars_errno = FAILURE_PVAR_EQUALS_UNKNOWN_VAR_TYPE;
			return false;
	}
}

pvar_t pvar_copy(const pvar_t *src)
{
	/* pvars_errno must be clear on each call to determine if pvar_copy fails recursively. See plist_copy */
	pvars_errno = PERRNO_CLEAR;
	pvar_t new_pvar;
	new_pvar.type = PVAR_TYPE_NONE;
	if (src == NULL) {
		pvars_errno = FAILURE_PVAR_COPY_NULL_INPUT;
		return new_pvar;
	}
	
	switch (src->type) {
		case PVAR_TYPE_STRING:
			/* Extra curly braces creates new scope for the char * declaration. Compilers with stricter standars should be satisfied */
			{
				char *new_string = strdup(src->data.s);
				if (new_string == NULL) {
					pvars_errno = FAILURE_PVAR_COPY_STRDUP_FAILED;
					return new_pvar;
				}
				new_pvar.data.s = new_string;
			}
			break;
		case PVAR_TYPE_LIST:
			/* Extra curly braces creates new scope for the plist_t * declaration. Compilers with stricter standars should be satisfied */
			{
				plist_t *new_list = plist_copy(src->data.ls);
				if (new_list == NULL) {
					pvars_errno = FAILURE_PVAR_COPY_PLIST_COPY_FAILED;
					return new_pvar;
				}
				new_pvar.data.ls = new_list;
			}
			break;
		//case PVAR_TYPE_DICT:
		//	break;
		//case PVAR_TYPE_TUPLE:
		//	break;
		case PVAR_TYPE_INT:
		case PVAR_TYPE_DOUBLE:
		case PVAR_TYPE_LONG:
		case PVAR_TYPE_FLOAT:
		case PVAR_TYPE_NONE:
			new_pvar.data = src->data;
			break;
		default:
			break;
	}
	new_pvar.type = src->type;
	
	pvars_errno = SUCCESS;
	return new_pvar;
}
