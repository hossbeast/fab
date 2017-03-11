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
 vertex

SUMMARY

REMARKS

*/

#include <stdint.h>
#include <sys/types.h>

struct graph;
struct edge;

typedef struct vertex
{
  const char *  label;    // (not owned)
  size_t        label_len;
  uint32_t      attrs;    // properties of the vertex

  struct list * up;       // edges where this == B, i.e. { up } -> this
  struct list * down;     // edges where this == A, i.e. this -> { down }

  int visited;            // id of the last traversal to visit this vertex

#ifndef VERTEX_INTERNALS
# define VERTEX_INTERNALS
#endif
  VERTEX_INTERNALS;
} vertex;

#define restrict __restrict

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
void * vertex_value(vertex * const restrict v)
  __attribute__((nonnull));

/// vertex_containerof
//
// SUMMARY
//  get a pointer to the vertex containing the user value
//
vertex * vertex_containerof(void * value)
  __attribute__((nonnull));

/// vertex_ascend
//
// SUMMARY
//  get the vertex above a vertex at a distance of 1 with the given label
//
// PARAMETERS
//  v   - starting vertex
//
// VARIANTS
//  edge - get the edge connecting the two vertices
//  s - label is given as a null terminated string
//  w - label is given as a buffer/length pair
//
// RETURNS
//  desired vertex or edge, if any
//
vertex * vertex_ascend(vertex * const restrict v, const char * const restrict label)
  __attribute__((nonnull));

struct edge * vertex_ascend_edge(vertex * const restrict v, const char * const restrict label)
  __attribute__((nonnull));

/// vertex_descend
//
// SUMMARY
//  get the vertex below a vertex at a distance of 1 with the given label
//
// PARAMETERS
//  v   - starting vertex
//
// VARIANTS
//  edge - get the edge connecting the two vertices
//  s - label is given as a null terminated string
//  w - label is given as a buffer/length pair
//
// RETURNS
//  desired vertex or edge, if any
//
vertex * vertex_descend(vertex * const restrict v, const char * const restrict label)
  __attribute__((nonnull));

struct edge * vertex_descend_edge(vertex * const restrict v, const char * const restrict label)
  __attribute__((nonnull));

#undef restrict
#endif
