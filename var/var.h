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
// pop all values off of a variables stack
//
// 
//
int var_undef(const map * const restrict vmap, const char * const restrict s, int * r)
	__attribute__((nonnull));

/// var_pop
//
// pop the top value off a variables stack, if any
//
int var_pop(const map * const restrict vmap, const char * const restrict s)
	__attribute__((nonnull));

/// var_push
//
// push an lstack or alias onto a variables stack
//
int var_push(const map * const restrict vmap, const char * const restrict s, const void * const restrict v, const uint8_t t, int sticky)
	__attribute__((nonnull(1, 2)));

/// var_container_free
//
// free a var_container with free semantics
//
int var_container_free(var_container * const restrict);

#undef restrict
#endif
