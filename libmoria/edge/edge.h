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

#ifndef _MORIA_EDGE_H
#define _MORIA_EDGE_H

#include "types.h"

struct moria_edge;
struct moria_graph;
struct moria_vertex;

/*
 * Get the non-hyper edge between the specified vertices
 */
struct moria_edge * moria_edge_between(const struct moria_vertex * restrict A, const struct moria_vertex * restrict B)
  __attribute__((nonnull));

/*
 * Get the edge connecting the specified sets of edges
 */
struct moria_edge * moria_edge_of(
    struct moria_vertex ** restrict Alist
  , uint16_t Alen
  , struct moria_vertex ** restrict Blist
  , uint16_t Blen
)
  __attribute__((nonnull));

void moria_edge_init(struct moria_edge * const restrict e, struct moria_graph * const restrict g)
  __attribute__((nonnull));

#endif
