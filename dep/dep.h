#ifndef _DEP_H
#define _DEP_H

#define restrict __restrict

/// dep_process
//
// SUMMARY
//  process FFN_DEPENDENCY nodes to create directed edges in the dependency graph
//
// PARAMETERS
//  ffn     - FFN_DEPENDENCY node
//  gn      - default needs-end node for dependencies lacking a lefthand side (discovery)
//  vmap    - vmap used to resolve variable references in the dependency lists
//  stax    - listwise stax
//  stax_l  - listwise stax
//  stax_a  - listwise stax
//  p       - offset to next available lstack
//  [first] - if not null, set to the needs-end of the first dependency processed
//  [newn]  - if not null, incremented by the number of new nodes created
//  [newr]  - if not null, incremented by the number of new edges created
//
int dep_process(
	  const ff_node * const restrict ffn
	, const gn * const restrict defgn
	, const map * const restrict vmap
	, lstack *** const restrict stax
	, int * const restrict staxl
	, int * const restrict staxa
	, int p
	, gn ** const restrict first
	, int * const restrict newn
	, int * const restrict newr
)
	__attribute__((nonnull(1,3,4,5,6)));

#endif
