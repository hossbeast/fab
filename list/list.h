/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _LIST_H
#define _LIST_H

#include <listwise.h>
#include <listwise/object.h>
#include <listwise/generator.h>

#include "ff.h"
#include "ffn.h"
#include "interpolate.h"

#include "map.h"
#include "pstring.h"

#define restrict __restrict

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
//  gp    - generator_parser for generators which are lists
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//  raw   - if true, interpret VARREF's as having a raw value (tue for FFN_FORMULA) instead of a container
//
int list_resolve(ff_node * restrict list, map * restrict vmap, generator_parser * const restrict gp, lstack *** restrict stax, int * restrict staxa, int * restrict staxp, int raw)
	__attribute__((nonnull));

/// list_resolveto
//
// SUMMARY
//  resolve an FFN_LIST into an lstack
//
// PARAMETERS
//  list  - FFN_LIST node
//  vmap  - map used to resolve variable references
//  gp    - generator_parser for generators which are lists
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//  raw   - if true, interpret VARREF's as having a raw value (tue for FFN_FORMULA) instead of a container
//
int list_resolveto(ff_node * restrict list, map * restrict vmap, generator_parser * const restrict gp, lstack *** restrict stax, int * restrict staxa, int * restrict staxp, int raw)
	__attribute__((nonnull));

/// list_resolveflat
//
// list_resolve
// list_flatten
//
int list_resolveflat(ff_node * restrict list, map * restrict vmap, generator_parser * const restrict gp, lstack *** restrict stax, int * restrict staxa, int * restrict staxp, int raw)
	__attribute__((nonnull));

/// list_resolvetoflat
//
// list_resolveto
// list_flatten
//
int list_resolvetoflat(ff_node * restrict list, map * restrict vmap, generator_parser * const restrict gp, lstack *** restrict stax, int * restrict staxa, int * restrict staxp, int raw)
	__attribute__((nonnull));

#undef restrict
#endif
