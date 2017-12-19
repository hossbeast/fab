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

#ifndef _MORIA_VERTEX_H
#define _MORIA_VERTEX_H

/*

MODULE
 moria/vertex

*/

#include <stdint.h>
#include <sys/types.h>

#include "types.h"
#include "xapi.h"

#include "traverse.h"

struct graph;
struct edge;

typedef struct vertex
{
  struct graph *  graph;    // graph that the vertex belongs to
  const char *    label;    // (not owned)
  size_t          label_len;
  uint32_t        attrs;    // properties of the vertex

  struct list *   up;       // edges where this == B, i.e. { up } -> this
  struct list *   down;     // edges where this == A, i.e. this -> { down }

  int visited;              // id of the last traversal to visit this vertex

#ifndef VERTEX_INTERNALS
# define VERTEX_INTERNALS
#endif
  VERTEX_INTERNALS;
} vertex;

/// vertex_value_set
//
// SUMMARY
//  set the value for the vertex
//
// REMARKS
//  size must match that provided in graph_create
//
// PARAMETERS
//  v       - pointer to vertex
//  [value] - pointer to value
//  vsz     - size of value
//
void vertex_value_set(vertex * const restrict v, void * value, size_t vsz)
  __attribute__((nonnull(1)));

/// vertex_value
//
// SUMMARY
//  get a pointer to the value for the vertex
//
void * vertex_value(const vertex * const restrict v)
  __attribute__((nonnull));

/// vertex_containerof
//
// SUMMARY
//  get a pointer to the vertex containing the user value
//
vertex * vertex_containerof(const void * value)
  __attribute__((nonnull));

/// vertex_travel
//
// SUMMARY
//  Get a vertex or edge at distance 1 from a starting vertex or edge. If a label is specified,
//  select a vertex or edge having that label. If no label is specified, then label is not considered.
//
// PARAMETERS
//  v              - starting vertex
//  [label]        - select a vertex with this label
//  [vertex_visit] - see traversal_criteria
//  [edge_visit]   - see traversal_criteria
//  attrs          - bitwise combination of MORIA_TRAVERSE_*
//
vertex * vertex_travel_vertex(
    const vertex * restrict v
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

vertex * vertex_travel_vertexs(
    const vertex * restrict v
  , const char * restrict label
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

vertex * vertex_travel_vertexw(
    const vertex * restrict v
  , const char * restrict label
  , size_t label_len
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

struct edge * vertex_travel_edge(
    const vertex * restrict v
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

struct edge * vertex_travel_edges(
    const vertex * restrict v
  , const char * restrict label
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

struct edge * vertex_travel_edgew(
    const vertex * restrict v
  , const char * restrict label
  , size_t label_len
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

#endif
