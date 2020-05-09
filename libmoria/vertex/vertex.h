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

#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "traverse.h"

struct graph;
struct edge;

#define MORIA_VERTEX_LINK   0x1 /* in attrs, vertex that refers to another vertex */

typedef struct vertex {
  const char *    label;        // (not owned)
  uint16_t        label_len;

  /* the bottom 6 bits are ignored for the purposes of ordering vertices in graph_say */
  uint32_t        attrs;        // properties of the vertex

  /* edges where this == B, i.e. { up } -> this */
  rbtree up;

  /* upwards identity edge - NOT stored in up */
  struct edge * up_identity;

  /* edges where this == A, i.e. this -> { down } */
  rbtree down;

  /* referent for VERTEX_LINK */
  struct vertex *ref;

  llist lln;            // user use
  llist graph_lln;      // graph list/freelist
} vertex;

/// vertex_create
//
// SUMMARY
//  allocate a vertex in the graph
//
// PARAMETERS
//  v      - (returns) vertex
//  g      - graph
//  attrs  - bitmask
//  label  - label for the vertex
//
// VARIANT
//  w - provide label as pointer/length pair
//
// REMARKS
//  the label is not copied or owned by the vertex
//
xapi vertex_create(struct vertex ** const restrict v, struct graph * const restrict g, uint32_t attrs)
  __attribute__((nonnull));

xapi vertex_creates(struct vertex ** const restrict v, struct graph * const restrict g, uint32_t attrs, const char * const restrict label)
  __attribute__((nonnull));

xapi vertex_createw(struct vertex ** const restrict v, struct graph * const restrict g, uint32_t attrs, const char * const restrict label, uint16_t label_len)
  __attribute__((nonnull));

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
//
void vertex_value_set(vertex * const restrict v, struct graph * restrict g, void * value)
  __attribute__((nonnull));

/// vertex_value
//
// SUMMARY
//  get a pointer to the value for the vertex
//
void * vertex_value(const vertex * const restrict v);

/// vertex_containerof
//
// SUMMARY
//  get a pointer to the vertex containing the user value
//
vertex * vertex_containerof(const void * value);

/*
 * Get the vertex at distance 1 from a starting vertex on the identity tree having the
 * specified label. These apis do not follow link nodes.
 *
 * v     - starting vertex
 * label - select the vertex with this label
 * attrs - one of MORIA_TRAVERSE_{UP,DOWN}
 */
vertex * vertex_downs(const vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

vertex * vertex_downw(const vertex * restrict v, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

struct edge * vertex_edge_downs(const vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct edge * vertex_edge_downw(const vertex * restrict v, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

vertex * vertex_up(const vertex * restrict v)
  __attribute__((nonnull));

struct edge * vertex_edge_up(const vertex * restrict v)
  __attribute__((nonnull));

#endif
