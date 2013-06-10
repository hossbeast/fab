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

#ifndef _DSC_H
#define _DSC_H

#include <listwise.h>

#include "gn.h"
#include "map.h"

#define restrict __restrict

/// dsc_exec
//
// SUMMARY
//  execute dependency discovery on a set of nodes, consisting of an initial set, plus
//  all dependencies needsward, minus any node having already participated in discovery
//
// PARAMETERS
//  roots   - graph nodes
//  rootsl  - len of gn
//  vmap    - map for resolving var references
//  gp      - generator parser
//  stax    - listwise stax
//  staxa   - listwise stax
//  staxp   - listwise stax
//  ts      - ts  (see ts_execwave)
//  tsa     - ts   |
//  tsw     - tsw  v
//  [new]   - if not null, incremented by the number of edges and nodes discovered
//
int dsc_exec(
	  gn ** restrict roots
	, int rootsl
	, map * restrict vmap
	, generator_parser * const restrict
	, lstack *** restrict stax
	, int * restrict staxa
	, int staxp
	, ts *** restrict ts
	, int * restrict tsa
	, int * restrict tsw
	, int * restrict new
)
	__attribute__((nonnull(1, 3, 4, 5, 6, 8, 9, 10)));

#undef restrict
#endif

