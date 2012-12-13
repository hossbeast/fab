#ifndef _DEP_H
#define _DEP_H

/// dep_add
//
// SUMMARY
//  create dependencies
//
// PARAMETERS
//  ffn     - FFN_DEPENDENCY node
//  vmap    - vmap used to resolve variable references in the dependency lists
//  stax    - listwise stax
//  stax_l  - listwise stax
//  stax_a  - listwise stax
//  p       - offset to next available lstack
//  [first] - if not null, set to the needs-end of the first dependency procelssed
//
int dep_add(ff_node * ffn, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p, gn ** first);

/// dep_add_bare
//
// SUMMARY
//  create dependencies
//
// PARAMETERS
//  target - needs-end of these dependencies
//  ffn    - FFN_DEPENDENCY node containing a list of targets
//
int dep_add_bare(gn * target, ff_node * ffn);

#endif
