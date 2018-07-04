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
 An identity graph is a tree where each vertex is uniquely identified by its path in the upward
 direction to another vertex called the root. For example, a filesystem tree is an identity graph
 with a root vertex at '/'.

*/

#include "xapi.h"
#include "types.h"

struct list;
struct map;
struct narrator;

struct vertex;
struct edge;
struct traversal_criteria;

typedef struct graph graph;

/// graph_create
//
// SUMMARY
//  create a graph
//
// PARAMETERS
//  g           - (returns) graph
//  [identity]  - nonzero to enable identity operations
//  [vsz]       - size of vertex udata
//  [vertex_value_destroy] - invoked on vertex user data before releasing
//
xapi graph_create(graph ** const restrict g, uint32_t identity)
  __attribute__((nonnull(1)));

xapi graph_createx(graph ** const restrict g, uint32_t identity, size_t vsz, void * vertex_value_destroy, void * vertex_value_xdestroy)
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

/// graph_recycle
//
// SUMMARY
//  reset the graph to its initial state, without releasing allocations
//
xapi graph_recycle(graph * const restrict g)
  __attribute__((nonnull));

/// graph_vertices
//
// SUMMARY
//  get a list of vertices in the graph
//
const struct list * graph_vertices(graph * restrict g)
  __attribute__((nonnull));

/// graph_edges
//
// SUMMARY
//  get a list of edges in the graph
//
const struct list * graph_edges(graph * restrict g)
  __attribute__((nonnull));

/// graph_say
//
// SUMMARY
//  write the normal representation of a graph to a narrator
//
// PARAMETERS
//  g   - graph
//  [d] - attrs name lookup
//  N   - narrator
//
xapi graph_say(graph * const restrict g, struct map * restrict definitions, struct narrator * const restrict N)
  __attribute__((nonnull(1, 3)));

/// graph_lookup_identifier_callback
//
// SUMMARY
//  returns the next label in the lookup sequence, or resets the lookup sequence if label == NULL
//
typedef xapi (*graph_lookup_identifier_callback)(
    void * context                  // user context
  , const char ** restrict label    // (returns) next label
  , uint16_t * restrict label_len   // (returns) next label length
);

/// graph_lookup
//
// SUMMARY
//  find a vertex in the identity tree
//
// PARAMETERS
//  g                   - graph with nonzero identity
//  identifier_callback - user callback to get vertex labels in the lookup sequence
//  context             - opaque user context for identifier_callback
//  mm_tmp              - required temp space for mm lookup
//  *V                  - (returns) one, or two vertices (based on *r)
//  *r                  - (returns) return code
//
// REMARKS
//  return code,
//   0 - no matching vertices
//   1 - one matching vertex (*V)
//   2 - two or more matching vertices (*V[0], *V[1])
//
xapi graph_lookup(
    struct graph * restrict g
  , graph_lookup_identifier_callback identifier_callback
  , void * context
  , void * mm_tmp
  , struct vertex * restrict V[2]
  , int * restrict r
)
  __attribute__((nonnull));

typedef struct graph_lookup_sentinel_context {
  char ** labels;     // sentinel-terminated sequence of labels
  uint16_t index;     // iteration state
} graph_lookup_sentinel_context;

/// graph_lookup_sentinel
//
// lookup callback for resolving an identifier comprised of a sentinel-terminated array of labels
//
xapi graph_lookup_sentinel(void * restrict context, const char ** restrict label, uint16_t * restrict label_len)
  __attribute__((nonnull(1)));

xapi graph_identity_indexs(graph * const restrict g, struct vertex * const restrict v, const char * const restrict name)
  __attribute__((nonnull));

xapi graph_identity_indexw(graph * const restrict g, struct vertex * const restrict v, const char * const restrict name, uint16_t name_len)
  __attribute__((nonnull));

#endif
