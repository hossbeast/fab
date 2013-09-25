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

#ifndef _VAR_H
#define _VAR_H

#include "listwise.h"
#include "listwise/object.h"

#include "map.h"

#define restrict __restrict

struct ff_node;

/// var_set
//
// SUMMARY
//  set an lstack as the definition for a variable
//
// PARAMETERS
//  vmap    - variable map
//  s       - target variable
//  ls      - lstack instance
//  inherit - whether to create an inherited definition
//  mutable - whether to create a mutable definition
//  [src]   - FFN_VAR* node (for logging)
//
// RETURNS
//  nonzero on success
//
int var_set(map * restrict vmap, const char * restrict s, lstack * const restrict ls, int inherit, int mutable, const struct ff_node * const restrict src)
	__attribute__((nonnull(1,2,3)));

/// var_xfm_add
//
// SUMMARY
//  add an lstack-addition-xfm to the definition for a variable
//
// PARAMETERS
//  vmap    - variable map
//  s       - target variable
//  ls      - addition
//  inherit - whether to create an inherited xfm
//  [src]   - FFN_VAR* node (for logging)
//
// RETURNS
//  nonzero on success
//
int var_xfm_add(map * restrict vmap, const char * restrict s, lstack * const restrict ls, int inherit, const struct ff_node * const restrict src)
	__attribute__((nonnull(1,2,3)));

/// var_sub
//
// SUMMARY
//  add an lstack-subtraction-xfm to the definition for a variable
//
// PARAMETERS
//  vmap    - variable map
//  s       - target variable
//  ls      - subtraction
//  inherit - whether to create an inherited xfm
//  [src]   - FFN_VAR* node (for logging)
//
// RETURNS
//  nonzero on success
//
int var_xfm_sub(map * restrict vmap, const char * restrict s, lstack * const restrict ls, int inherit, const struct ff_node * const restrict src)
	__attribute__((nonnull(1,2,3)));

/// var_xfm_lw
//
// SUMMARY
//  add an lw-xfm to the definition for a variable
//
// PARAMETERS
//  vmap    - variable map
//  s       - target variable
//  gen     - generator
//  tex     - generator string
//  inherit - whether to create an inherited xfm
//  [src]   - FFN_VAR* node (for logging)
//
// RETURNS
//  nonzero on success
//
int var_xfm_lw(map * restrict vmap, const char * restrict s, generator * const restrict xfm, char * const restrict tex, int inherit, const struct ff_node * const restrict src)
	__attribute__((nonnull(1,2,3,4)));

/// var_alias
//
// SUMMARY
//  create a one-way link between two variables in separate maps
//  aliases are always inherited
//
// PARAMETERS
//  amap    - source map
//  as      - source variable
//  bmap    - target map
//  bs      - target variable
//  src     - FFN_VAR* node (for logging)
//
// RETURNS
//  nonzero on success
//
int var_alias(map * const restrict amap, const char * const restrict as, map * const restrict bmap, const char * const restrict bs, const struct ff_node * const restrict src)
	__attribute__((nonnull));

/// var_link
//
// SUMMARY
//  create a two-way link between two variables in separate maps
//  links are always inherited
//
// PARAMETERS
//  amap    - source map
//  as      - source variable
//  bmap    - target map
//  bs      - target variable
//  src     - FFN_VAR* node (for logging)
//
// RETURNS
//  nonzero on success
//
int var_link(map * const restrict amap, const char * const restrict as, map * const restrict bmap, const char * const restrict bs, const struct ff_node * const restrict src)
	__attribute__((nonnull));

/// var_root
//
// create a root-level map
//
int var_root(map ** const restrict bmap)
	__attribute__((nonnull));

/// var_clone
//
// SUMMARY
//  create bmap with inherited entries from amap
//
int var_clone(map * const restrict amap, map ** const restrict bmap)
	__attribute__((nonnull));

/// var_access
//
// access the value for a given variable
//
// RETURNS
//  the definition, or listwise_identity if the variable is not defined, or has an empty definition stack
//
int var_access(const map * const restrict vmap, const char * restrict s, lstack *** const restrict stax, int * const restrict staxa, int * const restrict staxp, lstack ** const restrict ls)
	__attribute__((nonnull));

#undef restrict
#endif
