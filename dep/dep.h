#ifndef _DEP_H
#define _DEP_H

#include <listwise.h>

#include "ff.h"
#include "gn.h"
#include "depblock.h"

#define restrict __restrict

/// dep_process
//
// SUMMARY
//  process FFN_DEPENDENCY nodes to create directed edges in the dependency graph
//
// PARAMETERS
//  ffn        - FFN_DEPENDENCY node
//  vmap       - vmap used to resolve variable references in the dependency lists
//  stax       - listwise stax
//  staxa      - listwise stax
//  staxp      - offset to next available lstack
//  [first]    - if not null, set to the needs-end of the first dependency processed
//  [newn]     - if not null, incremented by the number of new nodes created
//  [newr]     - if not null, incremented by the number of new edges created
//  [block]    - if not null, attempt to copy all the dependencies into this dependency block
//
int dep_process(
	  ff_node * const restrict ffn
	, map * const restrict vmap
	, lstack *** const restrict stax
	, int * const restrict staxa
	, int staxp
	, gn ** const restrict first
	, int * const restrict newn
	, int * const restrict newr
	, depblock * const restrict block
)
	__attribute__((nonnull(1,2,3,4)));


/// depblock_process
//
// SUMMARY
//  process a depblock to create directed edges in the dependency graph
//
int depblock_process(
	  gn * const restrict dscvgn
	, const depblock * const restrict block
	, int * const restrict newnp
	, int * const restrict newrp
)
	__attribute__((nonnull(1,2)));

#undef restrict
#endif
