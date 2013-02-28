#ifndef _VAR_H
#define _VAR_H

#include <listwise.h>

#include "map.h"

#define VV_LS		1			// lstack
#define VV_AL		2			// alias

#define restrict __restrict

/// varval
//
// value assumed by a variable - either an lstack object, or an alias to another variable
//
// if a sticky value is at the top of the stack, then an undef/push is ignored
//
typedef struct
{
	int					sticky;
	uint8_t			type;		// one of { VV_LS, VV_AL }

	union
	{
		lstack *	ls;
		char *		alias;
	};
} varval;

/// var_container
//
// the values in vmap are of type var_container, which is a stack of varval
// when a variable is referenced, its value is whatever is at the top of its stack
//
typedef struct
{
	varval *	v;
	int 			l;
	int 			a;
} var_container;

/// var_undef
//
// SUMMARY
//  pop all values off of a variables stack
//
// PARAMETERS
//  vmap  - variable map
//  s     - target variable
//  r     - set to 1 if the variable was successfully cleared
//  stax  - stax
//  staxa - stax alloc
//  staxp - offset to next available stax
//
// RETURNS
//  nonzero on success
//
int var_undef(map * const restrict vmap, const char * const restrict s, int * r, lstack *** const restrict stax, int * const restrict staxa, int staxp)
	__attribute__((nonnull));

/// var_pop
//
// pop the top value off a variables stack, if any
//
int var_pop(map * const restrict vmap, const char * const restrict s, lstack *** const restrict stax, int * const restrict staxa, int staxp)
	__attribute__((nonnull));

/// var_push_list
//
// SUMMARY
//  take ownership of an lstack and push it onto a variables stack
//
// PARAMETERS
//  vmap   - variable map
//  s      - target variable
//  sticky - whether to create a sticky definition
//  stax   - stax
//  staxa  - stax alloc
//  staxp  - offset to stax containing the definition
//
// RETURNS
//  nonzero on success
//
int var_push_list(map * const restrict vmap, const char * const restrict s, int sticky, lstack *** const restrict stax, int * const restrict staxa, int staxp)
	__attribute__((nonnull(1, 2)));

/// var_push_alias
//
// SUMMARY
//  push an alias to another variable onto a variables stack
//
// PARAMETERS
//  vmap   - variable map
//  s      - target variable
//  sticky - whether to create a sticky definition
//  v      - name of aliased variable
//
// RETURNS
//  nonzero on success
//
int var_push_alias(map * const restrict vmap, const char * const restrict s, int sticky, char * const restrict v)
	__attribute__((nonnull));

/// var_access
//
// access the value for a given variable
//
// RETURNS
//  the definition, or listwise_identity if the variable is not defined, or has an empty definition stack
//
lstack * var_access(const map * const restrict vmap, const char * restrict s)
	__attribute__((nonnull));

/// var_container_free
//
// free a var_container with free semantics
//
void var_container_free(var_container * const restrict);

#undef restrict
#endif
