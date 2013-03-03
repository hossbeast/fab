#ifndef _LIST_H
#define _LIST_H

#include <listwise.h>

#include "ff.h"

#include "map.h"

/// list_ensure
//
// SUMMARY
//  ensure allocation of listwise stax up to staxp, reset entry @ stax[staxp]
//
// PARAMETERS
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//
int list_ensure(lstack *** stax, int * staxa, int staxp)
	__attribute__((nonnull));

/// list_resolveto
//
// SUMMARY
//  render an FFN_LIST node, appending to an lstack
//
// PARAMETERS
//  list  - FFN_LIST node
//  vmap  - map used to resolve variable references
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//  raw   - if true, interpret VARREF's as having a raw value (tue for FFN_FORMULA) instead of a container
//
int list_resolveto(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp, int raw)
	__attribute__((nonnull));

/// list_resolve
//
// SUMMARY
//  render an FFN_LIST node to an empty lstack
//
// PARAMETERS
//  list  - FFN_LIST node
//  vmap  - map used to resolve variable references
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//  raw   - if true, interpret VARREF's as having a raw value (tue for FFN_FORMULA) instead of a container
//
int list_resolve(ff_node * list, map* vmap, lstack *** stax, int * staxa, int staxp, int raw)
	__attribute__((nonnull));

#endif
