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

#ifndef _MORIA_GRAPH_INTERNAL_H
#define _MORIA_GRAPH_INTERNAL_H

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"

#include "graph.h"
#include "traverse.internal.h"

struct multimap;    // valyria/multimap
struct vertex_t;
struct edge_t;
struct attrs32;

struct graph {
  llist vertices;         // active
  llist vertex_freelist;  // deleted
  size_t vertex_index;    // traversal
  uint64_t vertex_mask;

  llist edges;            // active
  llist edge_freelist;    // deleted
  size_t edge_index;      // traversal
  uint64_t edge_mask;

  uint32_t identity;
  struct multimap * mm;
  llist states;           // traversal_state's
  const struct attrs32 * vertex_defs;
  const struct attrs32 * edge_defs;

  size_t vsz;     // vertex value size
  size_t esz;     // edge value size
  void (*vertex_value_destroy)(void *);
  xapi (*vertex_value_xdestroy)(void *);
};

void graph_vertex_init(graph * const restrict g, struct vertex_t * const restrict v)
  __attribute__((nonnull));

void graph_edge_init(graph * const restrict g, struct edge_t * const restrict v)
  __attribute__((nonnull));

#endif
