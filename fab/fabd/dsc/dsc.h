/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef _DSC_H
#define _DSC_H

#include "listwise.h"

#include "gn.h"
#include "map.h"

#define restrict __restrict

/// dsc_exec_entire
//
// SUMMARY
//  execute dependency discovery iteratively on the entire graph proceeding needsward
//
// PARAMETERS
//  vmap    - map for resolving var references
//  gp      - transform parser
//  stax    - listwise stax
//  staxa   - listwise stax
//  staxp   - listwise stax
//  ts      - ts  (see ts_execwave)
//  tsa     - ts   |
//  tsw     - tsw  v
//
int dsc_exec_entire(
	  map * restrict vmap
	, transform_parser * const restrict
	, lwx *** restrict stax
	, size_t * restrict staxa
	, size_t staxp
	, ts *** restrict ts
	, size_t * restrict tsa
	, size_t * restrict tsw
)
	__attribute__((nonnull(1, 2, 3, 4, 6, 7, 8)));

/// dsc_exec_specific
//
// SUMMARY
//  execute dependency discovery on a specific set of nodes
//
// PARAMETERS
//  list    - graph nodes
//  listl   - len of gn
//  vmap    - map for resolving var references
//  gp      - transform parser
//  stax    - listwise stax
//  staxa   - listwise stax
//  staxp   - listwise stax
//  ts      - ts  (see ts_execwave)
//  tsa     - ts   |
//  tsw     - tsw  v
//
int dsc_exec_specific(
	  gn *** const restrict list
	, size_t listl
	, map * restrict vmap
	, transform_parser * const restrict
	, lwx *** restrict stax
	, size_t * restrict staxa
	, size_t staxp
	, ts *** restrict ts
	, size_t * restrict tsa
	, size_t * restrict tsw
)
	__attribute__((nonnull(1, 3, 4, 5, 6, 8, 9, 10)));

#undef restrict
#endif
