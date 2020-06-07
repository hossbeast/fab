/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef _MORIA_TRAVERSE_H
#define _MORIA_TRAVERSE_H

/*

MODULE
 moria/traverse

*/

#include "xapi.h"
#include "types.h"

struct edge;
struct graph;
struct vertex;
struct attrs32;

#define TRAVERSAL_DIRECTION_OPT 0x00000003
#define TRAVERSAL_MODE_OPT      0x0000000c

/* traversal options and modifiers */
#define TRAVERSAL_DIRECTION_TABLE                                                                                                         \
  TRAVERSAL_ATTR_DEF(MORIA_TRAVERSE_UP   , TRAVERSAL_DIRECTION_OPT, 0x00000001) /* traverse from a node to its consumers (upward) */      \
  TRAVERSAL_ATTR_DEF(MORIA_TRAVERSE_DOWN , TRAVERSAL_DIRECTION_OPT, 0x00000002) /* traverse from a node to its dependencies (downward) */ \

#define TRAVERSAL_ATTR_DEF(x, r, y) x = UINT32_C(y),

typedef enum traversal_direction {
TRAVERSAL_DIRECTION_TABLE
} traversal_direction;

#define TRAVERSAL_MODE_TABLE                                                                              \
  TRAVERSAL_ATTR_DEF(MORIA_TRAVERSE_PRE  , TRAVERSAL_MODE_OPT, 0x00000004) /* breadth-first traversal */  \
  TRAVERSAL_ATTR_DEF(MORIA_TRAVERSE_POST , TRAVERSAL_MODE_OPT, 0x00000008) /* depth-first traversal */    \

typedef enum traversal_mode {
TRAVERSAL_MODE_TABLE
} traversal_mode;

extern struct attrs32 * traverse_attrs;
#undef TRAVERSAL_ATTR_DEF

#define MORIA_TRAVERSE_PRUNE      0x01 // prune this path in the traversal
#define MORIA_TRAVERSE_DEPTH      0x10 // use depth constraints in traversal_criteria
#define MORIA_TRAVERSE_EXHAUSTIVE 0x20 // permit entities to be visited multiple times
#define MORIA_TRAVERSE_NOFOLLOW   0x40 // do not follow link vertices to the referent

/// traversal_criteria
//
// SUMMARY
//  Parameters for selecting vertices and edges to consider during a traversal. An entity (a vertex, or an edge) is said
//  to be visited when the visitor callback is invoked on it. An entity is said to be traveled when entities on the far
//  side of it, from the perspective of the traversal, are considered as candidates for visitation.
//
typedef struct traversal_criteria {
  uint32_t vertex_travel;   // bitmask
  uint32_t vertex_visit;
  uint32_t edge_travel;
  uint32_t edge_visit;
  uint16_t min_depth;       // only if MORIA_TRAVERSE_DEPTH
  uint16_t max_depth;       // only if MORIA_TRAVERSE_DEPTH
} traversal_criteria;

struct vertex_traversal_state;
typedef struct vertex_traversal_state vertex_traversal_state;

struct edge_traversal_state;
typedef struct edge_traversal_state edge_traversal_state;

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
//  v|e            - vertex or edge at which to begin the traversal
//  [visitor]      - invoked on each visited vertex
//  [traversal_id] - see graph_traversal_begin
//  [criteria]     - vertex and edge selection bitmasks
//  attrs          - bitwise combination of MORIA_TRAVERSE_*
//  [ctx]          - optional user context
//
// VISITOR
//  v|e      - vertex or edge
//  distance - distance from the starting point
//  [ctx]    - user context
//
// REMARKS
//  An entity (a vertex, or an edge) is said to be traveled when it is considered as a candidate for
//  being visited. An entity is said to be visited when the visitor callback is invoked with the
//  entity as a parameter. When an entity is traveled, its neighbors are considered as candiates for
//  being traveled.
//
//  The default is to travel and visit each entity under consideration. The criteria parameter
//  specifies a set of bitmasks which can be used to restrict the set of entities which are traveled
//  and visited.
//
//  Note that in general, the only guarantee about the order in which entities are visited is that,
//  for a POSTORDER traversal, a vertex is visited after its neighbors.
//
xapi graph_traverse_vertices(
    /* 1 */ struct graph * restrict g
  , /* 2 */ struct vertex * restrict v
  , /* 3 */ xapi (* visitor)(struct vertex * restrict, void * ctx, traversal_mode mode, int distance, int * restrict)
  , /* 4 */ vertex_traversal_state * state
  , /* 5 */ const traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi graph_traverse_vertex_edges(
    /* 1 */ struct graph * restrict g
  , /* 2 */ struct vertex * restrict v
  , /* 3 */ xapi (* visitor)(struct edge * restrict, void * ctx, traversal_mode mode, int distance, int * restrict)
  , /* 4 */ edge_traversal_state * state
  , /* 5 */ const traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi graph_traverse_edges(
    /* 1 */ struct graph * restrict g
  , /* 2 */ struct edge * restrict e
  , /* 3 */ xapi (* visitor)(struct edge * restrict, void * ctx, traversal_mode mode, int distance, int * restrict)
  , /* 4 */ edge_traversal_state * state
  , /* 5 */ const traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi graph_traverse_edge_vertices(
    /* 1 */ struct graph * restrict g
  , /* 2 */ struct edge * restrict e
  , /* 3 */ xapi (* visitor)(struct vertex * restrict, void * ctx, traversal_mode mode, int distance, int * restrict)
  , /* 4 */ vertex_traversal_state * state
  , /* 5 */ const traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

/// graph_traverse_vertices_all
//
// SUMMARY
//  traverse all the entities of a graph while honoring directionality among entities
//
// PARAMETERS
//  as for graph_traverse
//
xapi graph_traverse_vertices_all(
    /* 1 */ struct graph * restrict g
  , /* 2 */ xapi (* visitor)(struct vertex * restrict, void * ctx, traversal_mode mode, int distance, int * restrict)
  , /* 3 */ const traversal_criteria * restrict criteria
  , /* 4 */ uint32_t attrs
  , /* 5 */ void * ctx
)
  __attribute__((nonnull(1)));

xapi graph_vertex_traversal_state_xfree(vertex_traversal_state * restrict state);
xapi graph_vertex_traversal_state_ixfree(vertex_traversal_state ** restrict state)
  __attribute__((nonnull));

xapi graph_edge_traversal_state_xfree(edge_traversal_state * restrict state);
xapi graph_edge_traversal_state_ixfree(edge_traversal_state ** restrict state)
  __attribute__((nonnull));

/// graph_traversal_begin
//
// SUMMARY
//  Open a traversal. Two traversals with the same id will not visit the same entity more than once.
//
// RETURNS
//  an id suitable for passing to graph_traverse_*
//
xapi graph_vertex_traversal_begin(struct graph * restrict g, vertex_traversal_state ** restrict state)
  __attribute__((nonnull));

xapi graph_edge_traversal_begin(struct graph * restrict g, edge_traversal_state ** restrict state)
  __attribute__((nonnull));

void graph_vertex_traversal_end(struct graph * restrict g, vertex_traversal_state * restrict state);
void graph_edge_traversal_end(struct graph * restrict g, edge_traversal_state * restrict state);

bool graph_traversal_vertex_visited(const struct graph * const restrict g, const struct vertex * const restrict v, vertex_traversal_state * restrict state)
  __attribute__((nonnull));

bool graph_traversal_edge_visited(const struct graph * const restrict g, const struct edge * const restrict v, edge_traversal_state * restrict state)
  __attribute__((nonnull));

#endif
