#ifndef _BAKE_H
#define _BAKE_H

#include <listwise.h>
#include <listwise/generator.h>

#include "bp.h"

#define restrict __restrict

/// bake_bp
//
// render the buildplan to a bakescript
//
int bake_bp(
	  const bp * const bp
	, map * const restrict vmap
	, generator_parser * const gp
	, lstack *** const restrict stax
	, int * const restrict staxa
	, int staxp
	, ts *** const restrict ts
	, int * const restrict tsa
	, const int * const restrict tsw
	, const char * const restrict dst
)
	__attribute__((nonnull));

#undef restrict
#endif
