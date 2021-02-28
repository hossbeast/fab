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

#ifndef _FABD_NODE_OPERATIONS_H
#define _FABD_NODE_OPERATIONS_H

#include "xapi.h"
#include "graph.h"

struct filesystem;
struct fsent;
struct dependency;
struct moria_vertex;
struct operations_dispatch;
struct moria_graph;
struct graph_invalidation_context;

xapi node_index(struct fsent * restrict n)
  __attribute__((nonnull));

xapi node_deindex(struct fsent * restrict n)
  __attribute__((nonnull));

xapi node_operations_create_vertex(
    struct moria_vertex ** const restrict rv
  , struct moria_graph * const restrict g
  , vertex_kind kind
  , uint32_t attrs
  , const char * const restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

#endif
