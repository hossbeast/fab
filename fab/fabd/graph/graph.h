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

#ifndef _GRAPH_H
#define _GRAPH_H

#include <sys/types.h>

/*

masters the dependency graph

*/

struct memblk;        // memblk.h
struct mempolicy;     // xlinux/mempolicy.h
struct gn;            // gn.h
struct map;           // map.h

#include "coll.h"

// this memblk contains exactly the entire graph
extern struct memblk * graph_memblk;

// mempolicy for graph_memblk
extern struct mempolicy * graph_mempolicy;

// the graph
extern union graph
{
	coll_doubly c;

	struct
	{
		size_t  l;					// length
		size_t  a;					// allocated
		size_t  z;					// element size

		struct gn ** e;			// elements

		struct map *	by_path;			// indexed by canonical path
		struct map *	by_pathhash;	// indexed by canonical path hash
	};
} graph;

#define restrict __restrict

/// graph_freeze
//
// SUMMARY
//  freeze the graph
//
void graph_freeze();

/// graph_thaw
//
// SUMMARY
//  thaw the graph
//
void graph_thaw();

/// graph_setup
//
// SUMMARY
//  
//
int graph_setup();

/// graph_teardown
//
// SUMMARY
//  
//
void graph_teardown();

#undef restrict
#endif
