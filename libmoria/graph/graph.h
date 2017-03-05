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
struct edge;
struct narrator;
struct list;

typedef struct graph graph;

#define restrict __restrict

/// graph_create
//
// SUMMARY
//  create a graph
//
// PARAMETERS
//  g     - (returns) graph
//  [vsz] - size of vertex udata
//  [vertex_value_destroy] - invoked on vertex user data before releasing
//
xapi graph_create(graph ** const restrict g)
  __attribute__((nonnull(1)));

xapi graph_createx(graph ** const restrict g, size_t vsz, void * vertex_value_destroy, void * vertex_value_xdestroy)
  __attribute__((nonnull(1)));

/// graph_say
//
// SUMMARY
//  write a sorted list of edges to the narrator
//
xapi graph_say(graph * const restrict g, struct narrator * const restrict N)
  __attribute__((nonnull));

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
//  v         - (returns) vertex
//  g         - graph
//  attrs     - bitmask
//
// VARIANT
//  w - provide label as pointer/length pair
//
// REMARKS
//  the label is not copied or owned by the vertex
//
xapi graph_vertex_create(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs)
  __attribute__((nonnull));

xapi graph_vertex_createw(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs, const char * const restrict label, size_t label_len)
  __attribute__((nonnull));

/// graph_connect_edge
//
// SUMMARY
//  create the edge A : B, if it does not already exist, with the specified attributes
//
// REMARKS
//  A : B, e.g. A needs B, e.g. A depends on B, e.g. A is up from B, B is down from A
//
// PARAMETERS
//  g     - graph
//  A     - vertex
//  B     - vertex
//
xapi graph_connect_edge(graph * const restrict g, struct vertex * A, struct vertex * B, uint32_t attrs)
  __attribute__((nonnull));

xapi graph_disconnect_edge(graph * const restrict g, struct vertex * A, struct vertex * B)
  __attribute__((nonnull));

/// GRAPH_TRAVERSE_TABLE
//
// SUMMARY
//  graph traversal options and modifiers
//
#define GRAPH_TRAVERSE_TABLE(x, y)                                                                               \
  GRAPH_TRAVERSE_DEF(UP       , 0x00000001 , x , y)  /* traverse from a node to its consumers (upward) */        \
  GRAPH_TRAVERSE_DEF(DOWN     , 0x00000002 , x , y)  /* traverse from a node to its dependencies (downward) */   \
  GRAPH_TRAVERSE_DEF(PRE      , 0x00000004 , x , y)  /* breadth-first traversal */                               \
  GRAPH_TRAVERSE_DEF(POST     , 0x00000008 , x , y)  /* depth-first traversal */

enum {
#define GRAPH_TRAVERSE_DEF(a, b, x, y) GRAPH_TRAVERSE_ ## a = UINT32_C(b),
GRAPH_TRAVERSE_TABLE(0, 0)
#undef GRAPH_TRAVERSE_DEF
};

/// graph_traverse_vertices
//
// SUMMARY
//  traverse the vertices of a graph
//
// PARAMETERS
//  g               - graph
//  v               - vertex at which to begin the traversal
//  visitor         - invoked on each visited vertex
//  [traversal_id]  -
//  [travel]        - see remarks
//  [visit]         -
//  attrs           - bitwise combination of GRAPH_TRAVERSE_*
//  [ctx]           - optional context
//
// VISITOR
//  v     -
//  distance -
//  [ctx] -
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
xapi graph_traverse_vertices(
    /* 1 */ graph * const restrict g
  , /* 2 */ struct vertex * const restrict v
  , /* 3 */ xapi (* const visitor)(struct vertex * const restrict, int distance, void *)
  , /* 4 */ int traversal_id
  , /* 5 */ uint32_t vertex_travel
  , /* 6 */ uint32_t vertex_visit
  , /* 7 */ uint32_t edge_travel
  , /* 8 */ uint32_t edge_visit
  , /* 9 */ uint32_t attrs
  , /* a */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi graph_traverse_edges(
    /* 1 */ graph * const restrict g
  , /* 2 */ struct edge * const restrict e
  , /* 3 */ xapi (* const visitor)(struct edge * const restrict, int distance, void *)
  , /* 4 */ int traversal_id
  , /* 5 */ uint32_t vertex_travel
  , /* 6 */ uint32_t vertex_visit
  , /* 7 */ uint32_t edge_travel
  , /* 8 */ uint32_t edge_visit
  , /* 9 */ uint32_t attrs
  , /* a */ void * ctx
)
  __attribute__((nonnull(1, 2)));

/// graph_traversal_begin
//
// SUMMARY
//  open a traversal
//
// RETURNS
//  an id suitable for passing to graph_traverse
//
int graph_traversal_begin(graph * const restrict g)
  __attribute__((nonnull));

struct list * graph_vertices(graph * const restrict g)
  __attribute__((nonnull));

struct list * graph_edges(graph * const restrict g)
  __attribute__((nonnull));

#undef restrict
#endif
