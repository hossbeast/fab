#ifndef _DEP_H
#define _DEP_H

#include <listwise.h>

#include "ffn.h"
#include "gn.h"
#include "depblock.h"

#include "map.h"
#include "strstack.h"

#define restrict __restrict

/// dep_process
//
// SUMMARY
//  process FFN_DEPENDENCY nodes to create directed edges in the dependency graph
//
// PARAMETERS
//  ffn        - FFN_DEPENDENCY node
//  [sstk]     - sstk used to resolve nofile paths (not required for dependencies in an autodisc fabfile)
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
	, strstack * const restrict sstk
	, map * const restrict vmap
	, lstack *** const restrict stax
	, int * const restrict staxa
	, int staxp
	, gn ** const restrict first
	, int * const restrict newn
	, int * const restrict newr
	, depblock * const restrict block
)
	__attribute__((nonnull(1,3,4,5)));

#undef restrict
#endif
