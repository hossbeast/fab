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

#ifndef _MORIA_GRAPH_H
#define _MORIA_GRAPH_H

/*

MODULE
 graph

SUMMARY

REMARKS

*/

#include "xapi.h"

struct vertex;
struct narrator;
struct graph;
typedef struct graph graph;

#define restrict __restrict

/// graph_create
//
// SUMMARY
//  create a graph
//
// PARAMETERS
//  g     - (returns) graph
//  [say] - callback function when a cycle is detected to get a node description
//
xapi graph_create(graph ** const restrict g, xapi (* say)(struct vertex * const restrict v, struct narrator * const restrict N))
  __attribute__((nonnull(1)));

/// graph_xfree
//
// SUMMARY
//  free a graph
//
xapi graph_xfree(graph * const restrict g);

/// graph_ifree
//
// SUMMARY
//  free a graph, zero its reference
//
xapi graph_ixfree(graph ** const restrict g)
  __attribute__((nonnull));

/// graph_vertex_create
//
// SUMMARY
//  allocate a vertex in the graph
//
// PARAMETERS
//  g     - graph
//  v     - (returns) vertex
//
xapi graph_vertex_create(graph * const restrict g, struct vertex ** const restrict v)
  __attribute__((nonnull));

/// graph_relate
//
// SUMMARY
//  create the edge A -> B, if it does not already exist, with the specified attributes
//
// PARAMETERS
//  g     - graph
//  A     - vertex
//  B     - vertex
//  attrs -
//
xapi graph_relate(graph * const restrict g, struct vertex * A, struct vertex * B, uint32_t attrs)
  __attribute__((nonnull));

/// GRAPH_TRAVERSE_TABLE
//
// SUMMARY
//  graph traversal options and modifiers
//
#define GRAPH_TRAVERSE_TABLE(x, y)                                                                                    \
  GRAPH_TRAVERSE_DEF(UP            , 0x00000001 , x , y)  /* traverse from a node to its consumers (upward) */        \
  GRAPH_TRAVERSE_DEF(DOWN          , 0x00000002 , x , y)  /* traverse from a node to its dependencies (downward) */   \
  GRAPH_TRAVERSE_DEF(DEPTH         , 0x00000004 , x , y)  /* depth-first traversal */                                 \
  GRAPH_TRAVERSE_DEF(BREADTH       , 0x00000008 , x , y)  /* breadth-first traversal */

enum {
#define GRAPH_TRAVERSE_DEF(a, b, x, y) GRAPH_TRAVERSE_ ## a = UINT32_C(b),
GRAPH_TRAVERSE_TABLE(0, 0)
#undef GRAPH_TRAVERSE_DEF
};

/// graph_traverse
//
// SUMMARY
//  traverse the graph
//
// PARAMETERS
//  g        - graph
//  v        - vertex at which to begin the traversal
//  visit    - callback function to visit a node
//  skip     - see remarks
//  stop     - see remarks
//  finish   - see remarks
//  attrs    - bitwise combination of GRAPH_TRAVERSE_*
//  [arg]    - optional context
//
// REMARKS
//  For each node, the default operation is to visit that node, and continue by traversing its
//  neighbors. The arguments skip, stop, and finish are bitmasks which alter this behavior, as
//  follows.
//
//  For an edge whose attrs matches one of the masks, the traversal proceeds along the edge
//  according to the table below. If the attrs of an edge match more than one of the bitmasks, the
//  behavior is undefined. For the initial node, it is as if traverse and visit are both true
//
//               traverse    visit
//            ---------------------
//  stop      |
//  finish    |                x
//  skip      |     x
//  (default) |     x          x
//
xapi graph_traverse(
    /* 1 */ graph * const restrict g
  , /* 2 */ struct vertex * const restrict v
  , /* 2 */ xapi (* const visit)(struct vertex * const restrict, void *)
  , /* 3 */ uint32_t skip
  , /* 4 */ uint32_t finish
  , /* 5 */ uint32_t stop
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * arg
)
  __attribute__((nonnull(1, 2, 3)));

#undef restrict
#endif
