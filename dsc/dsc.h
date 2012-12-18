#ifndef _DSC_H
#define _DSC_H

#include <listwise.h>

#include "gn.h"

#include "map.h"

#define restrict __restrict

/// dsc_exec
//
// SUMMARY
//  execute dependency discovery on a set of nodes, consisting of an initial set, plus
//  all dependencies needsward, minus any node having already participated in discovery
//
// PARAMETERS
//  roots   - graph nodes
//  rootsl  - len of gn
//  prune   - if true, skip nodes which have not been marked as changed (which occurs in bp_prune)
//  vmap    - map for resolving var references
//  stax    - listwise stax
//  staxl   - listwise stax
//  staxa   - listwise stax
//  staxp   - listwise stax
//  ts      - ts  (see ts_execwave)
//  tsa     - ts   |
//  tsw     - tsw  v
//  [new]   - if not null, incremented by the number of edges and nodes discovered
//
int dsc_exec(
	  gn ** restrict roots
	, int rootsl
	, int prune
	, map * restrict vmap
	, lstack *** restrict stax
	, int * restrict staxl
	, int * restrict staxa
	, int staxp
	, ts *** restrict ts
	, int * restrict tsa
	, int * restrict tsw
	, int * restrict new
)
	__attribute__((nonnull(1, 4, 5, 6, 7, 9, 10, 11)));

#endif
