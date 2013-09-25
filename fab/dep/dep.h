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

#ifndef _DEP_H
#define _DEP_H

#include "listwise.h"

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
	, generator_parser * const gp
	, lstack *** const restrict stax
	, int * const restrict staxa
	, int staxp
	, gn ** const restrict first
	, int * const restrict newn
	, int * const restrict newr
	, depblock * const restrict block
)
	__attribute__((nonnull(1,3,4,5,6)));

#undef restrict
#endif

