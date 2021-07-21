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

#include "types.h"

struct moria_graph;
struct moria_vertex;
struct moria_edge;

/// vertex_init
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
//  the label is not copied into or owned by the vertex
//
void moria_vertex_init(struct moria_vertex * const restrict v, struct moria_graph * const restrict g, uint32_t attrs)
  __attribute__((nonnull));

void moria_vertex_inits(struct moria_vertex * const restrict v, struct moria_graph * const restrict g, uint32_t attrs, const char * const restrict label)
  __attribute__((nonnull));

void moria_vertex_initw(struct moria_vertex * const restrict v, struct moria_graph * const restrict g, uint32_t attrs, const char * const restrict label, uint16_t label_len)
  __attribute__((nonnull));

/*
 * Get the vertex at distance 1 from a starting vertex on the identity tree having the
 * specified label. These apis do not follow link nodes.
 *
 * v     - starting vertex
 * label - select the vertex with this label
 * attrs - one of MORIA_TRAVERSE_{UP,DOWN}
 */
struct moria_vertex * moria_vertex_downs(const struct moria_vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_downw(const struct moria_vertex * restrict v, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

struct moria_edge * moria_vertex_edge_downs(const struct moria_vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct moria_edge * moria_vertex_edge_downw(const struct moria_vertex * restrict v, const char * restrict label, uint16_t label_len)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_up(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

struct moria_edge * moria_vertex_edge_up(const struct moria_vertex * restrict v)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_descends(const struct moria_vertex * restrict v, const char * restrict label)
  __attribute__((nonnull));

struct moria_vertex * moria_vertex_descendw(const struct moria_vertex * restrict v, const char * restrict path, uint16_t path_len)
  __attribute__((nonnull));

#endif
