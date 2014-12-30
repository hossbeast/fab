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

#ifndef _FABD_DEP_H
#define _FABD_DEP_H

struct ff_node;
struct strstack;
struct map;
struct transform_parser;
struct lwx;
struct gn;

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
//
int dep_process(
	  struct ff_node * const restrict ffn
	, struct strstack * const restrict sstk
	, struct map * const restrict vmap
	, struct transform_parser * const gp
	, struct lwx *** const restrict stax
	, int * const restrict staxa
	, int staxp
	, struct gn ** const restrict first
	, int * const restrict newn
	, int * const restrict newr
)
	__attribute__((nonnull(1,3,4,5,6)));

#undef restrict
#endif

