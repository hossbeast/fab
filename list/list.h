#ifndef _LIST_H
#define _LIST_H

#include <listwise.h>
#include <listwise/object.h>

#include "ff.h"

#include "map.h"
#include "pstring.h"

#define restrict __restrict

// listwise interface to list objects
//  (this type is a placeholder - instances of this object are NOT ever passed into liblistwise)
listwise_object listlw;

#define LISTWISE_TYPE_LIST	0x02		/* listwise type id */

/*
** you "resolve" an FFN_LIST node to an lstack*
**  - resolve variable references
**  - resolve a generator, if any
**  - if there is a generator, flatten first
**  ex: 
**   - dependency targets
**   - formula targets
**
** you "render" an lstack* to a string
**  - rendering has a delimiter
**  - embedded lists have their own delimiter
**  ex:
**   - interpolation into formula text
**   - interpretaion as a filepath for an invocation
*/

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

/// list_flatten
//
// replace LISTWISE_TYPE_LIST objects with their contents
//
int list_flatten(lstack * restrict ls)
	__attribute__((nonnull));

/// list_render
//
// SUMMARY
//  render an lstack object to an empty pstring
//
// PARAMETERS
//  ls  - lstack object
//  ps  - pstring
//
int list_render(lstack * const restrict ls, pstring ** const restrict ps)
	__attribute__((nonnull));

/// list_renderto
//
// SUMMARY
//  render an lstack object, appending to a pstring
//
// PARAMETERS
//  ls  - lstack object
//  ps  - pstring
//
int list_renderto(lstack * const restrict ls, pstring ** const restrict ps)
	__attribute__((nonnull));

/// list_resolve
//
// SUMMARY
//  reset an lstack, then resolve an FFN_LIST into that lstack
//
// PARAMETERS
//  list  - FFN_LIST node
//  vmap  - map used to resolve variable references
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//  raw   - if true, interpret VARREF's as having a raw value (tue for FFN_FORMULA) instead of a container
//
int list_resolve(ff_node * restrict list, map * restrict vmap, lstack *** restrict stax, int * restrict staxa, int * restrict staxp, int raw)
	__attribute__((nonnull));

/// list_resolveto
//
// SUMMARY
//  resolve an FFN_LIST into an lstack
//
// PARAMETERS
//  list  - FFN_LIST node
//  vmap  - map used to resolve variable references
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//  raw   - if true, interpret VARREF's as having a raw value (tue for FFN_FORMULA) instead of a container
//
int list_resolveto(ff_node * restrict list, map * restrict vmap, lstack *** restrict stax, int * restrict staxa, int * restrict staxp, int raw)
	__attribute__((nonnull));

/// list_resolveflat
//
// list_resolve
// list_flatten
//
int list_resolveflat(ff_node * restrict list, map * restrict vmap, lstack *** restrict stax, int * restrict staxa, int staxp)
	__attribute__((nonnull));

/// list_resolvetoflat
//
// list_resolveto
// list_flatten
//
int list_resolvetoflat(ff_node * restrict list, map * restrict vmap, lstack *** restrict stax, int * restrict staxa, int staxp)
	__attribute__((nonnull));

#undef restrict
#endif
