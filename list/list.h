#ifndef _LIST_H
#define _LIST_H

#include <listwise.h>

#include "ff.h"

#include "map.h"

/// list_ensure
//
// SUMMARY
//  ensure allocation of listwise stax, reset entry @ stax[p]
//
// PARAMETERS
//  stax  - listwise stax
//  staxl - listwise stax
//  staxa - listwise stax
//  p     - offset to next free stax
//
int list_ensure(lstack *** stax, int * staxl, int * staxa, int p)
	__attribute__((nonnull));

/// list_resolve
//
// SUMMARY
//  render an FFN_LIST node to an lstack
//
// PARAMETERS
//  list  - FFN_LIST node
//  vmap  - map used to resolve variable references
//  stax  - listwise stax
//  staxl - listwise stax
//  staxa - listwise stax
//  p     - offset to next free stax
//
int list_resolve(ff_node * list, map* vmap, lstack *** stax, int * staxl, int * staxa, int p)
	__attribute__((nonnull));

#endif
