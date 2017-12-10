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

#include <sys/types.h>
#include <stdint.h>
#include "xapi.h"
#include "traverse.h"

struct vertex;
struct edge;
struct narrator;
struct list;
struct traversal_criteria;

typedef struct graph graph;

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
//  write a sorted list of edges to a narrator
//
// PARAMETERS
//  g            - graph
//  [edge_visit] - if nonzero, only use edges which match the mask
//  N            - narrator
//
xapi graph_say(graph * const restrict g, uint32_t edge_visit, struct narrator * const restrict N)
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

/// graph_recycle
//
// SUMMARY
//  reset the graph to its initial state, without releasing allocations
//
xapi graph_recycle(graph * const restrict g)
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

xapi graph_vertex_creates(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs, const char * const restrict label)
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

/// graph_traverse
//
// SUMMARY
//  traverse a graph from a starting point
//
// VARIANTS
//  vertices - visitor invoked on vertices
//  edges - visitor invoked on edges
//
// PARAMETERS
//  g              - graph
//  v              - vertex at which to begin the traversal
//  [visitor]      - invoked on each visited vertex
//  [traversal_id] - see graph_traversal_begin
//  [criteria]     - vertex and edge selection bitmasks
//  attrs          - bitwise combination of MORIA_TRAVERSE_*
//  [ctx]          - optional user context
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
    /* 1 */ graph * restrict g
  , /* 2 */ struct vertex * restrict v
  , /* 3 */ xapi (* visitor)(struct vertex * restrict, int distance, void *)
  , /* 4 */ int traversal_id
  , /* 5 */ const struct traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi graph_traverse_edges(
    /* 1 */ graph * restrict g
  , /* 2 */ struct edge * restrict e
  , /* 3 */ xapi (* visitor)(struct edge * restrict, int distance, void *)
  , /* 4 */ int traversal_id
  , /* 5 */ const struct traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

/// graph_traverse_all
//
// SUMMARY
//  traverse all the entities of a graph while honoring directionality among entities
//
// PARAMETERS
//  as for graph_traverse
//
xapi graph_traverse_all_vertices(
    /* 1 */ graph * restrict g
  , /* 2 */ xapi (* visitor)(struct vertex * restrict, int distance, void *)
  , /* 3 */ const struct traversal_criteria * restrict criteria
  , /* 4 */ uint32_t attrs
  , /* 5 */ void * ctx
)
  __attribute__((nonnull(1)));

/// graph_traversal_begin
//
// SUMMARY
//  Open a traversal. Two traversals with the same id will not visit the same entity more than once.
//
// RETURNS
//  an id suitable for passing to graph_traverse_*
//
int graph_traversal_begin(graph * restrict g)
  __attribute__((nonnull));

struct list * graph_vertices(graph * restrict g)
  __attribute__((nonnull));

struct list * graph_edges(graph * restrict g)
  __attribute__((nonnull));

#endif
