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
	  gn ** restrict n
	, int gnl
	, map * restrict vmap
	, lstack *** restrict stax
	, int * restrict staxl
	, int * restrict staxa
	, int p
	, ts *** restrict ts
	, int * restrict tsa
	, int * restrict tsw
)
	__attribute__((nonnull));

#endif
