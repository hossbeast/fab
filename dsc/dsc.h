#ifndef _DSC_H
#define _DSC_H

#include <listwise.h>

#include "gn.h"

#include "map.h"

#define restrict __restrict

/// dsc_exec
//
// execute dependency discovery
//
int dsc_exec(
	  gn * restrict n
	, map * restrict vmap
	, lstack *** restrict stax
	, int * restrict stax_l
	, int * restrict stax_a
	, int p
	, ts *** restrict ts
	, int * restrict tsa
)
	__attribute__((nonnull));

#endif
