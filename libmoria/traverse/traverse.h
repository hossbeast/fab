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

#include "xapi.h"
#include "types.h"

struct moria_edge;
struct moria_graph;
struct moria_vertex;
struct attrs32;

#define MORIA_TRAVERSAL_DIRECTION_OPT 0x00000003
#define MORIA_TRAVERSAL_MODE_OPT      0x0000000c

/* traversal options and modifiers */
#define MORIA_TRAVERSAL_DIRECTION_TABLE                                                                                          \
  DEF(MORIA_TRAVERSE_UP   , MORIA_TRAVERSAL_DIRECTION_OPT, 0x00000001) /* traverse from a node to its consumers (upward) */      \
  DEF(MORIA_TRAVERSE_DOWN , MORIA_TRAVERSAL_DIRECTION_OPT, 0x00000002) /* traverse from a node to its dependencies (downward) */ \

typedef enum moria_traversal_direction {
#undef DEF
#define DEF(x, r, y) x = UINT32_C(y),
MORIA_TRAVERSAL_DIRECTION_TABLE
} moria_traversal_direction;

#define MORIA_TRAVERSAL_MODE_TABLE                                                               \
  DEF(MORIA_TRAVERSE_PRE  , MORIA_TRAVERSAL_MODE_OPT, 0x00000004) /* breadth-first traversal */  \
  DEF(MORIA_TRAVERSE_POST , MORIA_TRAVERSAL_MODE_OPT, 0x00000008) /* depth-first traversal */    \

typedef enum moria_traversal_mode {
#undef DEF
#define DEF(x, r, y) x = UINT32_C(y),
MORIA_TRAVERSAL_MODE_TABLE
} moria_traversal_mode;

extern struct attrs32 * moria_traverse_attrs;

#define MORIA_TRAVERSE_PRUNE      0x01 // prune this path in the traversal (PREORDER only)
#define MORIA_TRAVERSE_DEPTH      0x10 // use depth constraints in moria_traversal_criteria
#define MORIA_TRAVERSE_EXHAUSTIVE 0x20 // permit entities to be visited multiple times
#define MORIA_TRAVERSE_NOFOLLOW   0x40 // do not follow link vertices to the referent

/// moria_traversal_criteria
//
// SUMMARY
//  Parameters for selecting vertices and edges to consider during a traversal. An entity (a vertex, or an edge) is said
//  to be visited when the visitor callback is invoked on it. An entity is said to be traveled when entities on the far
//  side of it, from the perspective of the traversal, are considered as candidates for visitation.
//
typedef struct moria_traversal_criteria {
  uint32_t vertex_travel;   // bitmask
  uint32_t vertex_visit;
  uint32_t edge_travel;
  uint32_t edge_visit;
  uint16_t min_depth;       // only if MORIA_TRAVERSE_DEPTH
  uint16_t max_depth;       // only if MORIA_TRAVERSE_DEPTH
} moria_traversal_criteria;

struct moria_vertex_traversal_state;
typedef struct moria_vertex_traversal_state moria_vertex_traversal_state;

struct moria_edge_traversal_state;
typedef struct moria_edge_traversal_state moria_edge_traversal_state;

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
//  g           - graph
//  v|e         - vertex or edge at which to begin the traversal
//  [visitor]   - invoked on each visited vertex
//  [state]     - see graph_traversal_begin
//  [criteria]  - vertex and edge selection bitmasks
//  attrs       - bitwise combination of MORIA_TRAVERSE_*
//  [ctx]       - optional user context
//
// VISITOR
//  v|e      - vertex or edge
//  [ctx]    - user context
//  distance - distance from the starting point
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
//  for a MORIA_TRAVERSE_POST traversal, a vertex is visited after its neighbors.
//
xapi moria_traverse_vertices(
    /* 1 */ struct moria_graph * restrict g
  , /* 2 */ struct moria_vertex * restrict v
  , /* 3 */ xapi (* visitor)(struct moria_vertex * restrict, void * ctx, moria_traversal_mode mode, int distance, int * restrict)
  , /* 4 */ moria_vertex_traversal_state * state
  , /* 5 */ const moria_traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi moria_traverse_vertex_edges(
    /* 1 */ struct moria_graph * restrict g
  , /* 2 */ struct moria_vertex * restrict v
  , /* 3 */ xapi (* visitor)(struct moria_edge * restrict, void * ctx, moria_traversal_mode mode, int distance, int * restrict)
  , /* 4 */ moria_edge_traversal_state * state
  , /* 5 */ const moria_traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi moria_traverse_edges(
    /* 1 */ struct moria_graph * restrict g
  , /* 2 */ struct moria_edge * restrict e
  , /* 3 */ xapi (* visitor)(struct moria_edge * restrict, void * ctx, moria_traversal_mode mode, int distance, int * restrict)
  , /* 4 */ moria_edge_traversal_state * state
  , /* 5 */ const moria_traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

xapi moria_traverse_edge_vertices(
    /* 1 */ struct moria_graph * restrict g
  , /* 2 */ struct moria_edge * restrict e
  , /* 3 */ xapi (* visitor)(struct moria_vertex * restrict, void * ctx, moria_traversal_mode mode, int distance, int * restrict)
  , /* 4 */ moria_vertex_traversal_state * state
  , /* 5 */ const moria_traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

/*
 * Callback to count vertices visited - ctx points to a uint32_t
 */
xapi moria_vertex_count(
    struct moria_vertex * restrict v
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , int * restrict
)
  __attribute__((nonnull(2)));

xapi moria_vertex_count_once(
    struct moria_vertex * restrict v
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , int * restrict
)
  __attribute__((nonnull(2, 5)));

/*
 * Callback to count edges visited - ctx points to a uint32_t
 */
xapi moria_edge_count(
    struct moria_edge * restrict v
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , int * restrict
)
  __attribute__((nonnull(2)));

xapi moria_edge_count_once(
    struct moria_edge * restrict v
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , int * restrict
)
  __attribute__((nonnull(2, 5)));

/*
 * Open a traversal. Two traversals with the same state will not visit the same entity more than once.
 */
xapi moria_vertex_traversal_begin(struct moria_graph * restrict g, moria_vertex_traversal_state ** restrict state)
  __attribute__((nonnull));

xapi moria_edge_traversal_begin(struct moria_graph * restrict g, moria_edge_traversal_state ** restrict state)
  __attribute__((nonnull));

/*
 * Close a traversal. The state may then be reused.
 */
void moria_vertex_traversal_end(struct moria_graph * restrict g, moria_vertex_traversal_state * restrict state);
void moria_edge_traversal_end(struct moria_graph * restrict g, moria_edge_traversal_state * restrict state);

bool moria_traversal_vertex_visited(
    const struct moria_graph * const restrict g
  , const struct moria_vertex * const restrict v
  , moria_vertex_traversal_state * restrict state
)
  __attribute__((nonnull));

bool moria_traversal_edge_visited(
    const struct moria_graph * const restrict g
  , const struct moria_edge * const restrict v
  , moria_edge_traversal_state * restrict state
)
  __attribute__((nonnull));

#endif
