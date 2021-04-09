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

#ifndef FABD_FSEDGE_H
#define FABD_FSEDGE_H

/*

fsedge objects are edges in the graph which represent the filesystem tree

*/

#include "xapi.h"
#include "types.h"
#include "valyria/llist.h"
#include "moria.h"

#include "graph.h"

struct fsent;
struct graph_invalidation_context;

// active list
extern llist fsedge_list;

typedef struct fsedge {
  moria_edge edge;
} fsedge;

/* release edges */
xapi fsedge_cleanup(void);

xapi fsedge_alloc(fsedge ** restrict rv, struct moria_graph * restrict g)
  __attribute__((nonnull));

void fsedge_release(fsedge * restrict dep)
  __attribute__((nonnull));

xapi fsedge_connect(
    struct fsent * restrict A
  , struct fsent * restrict B
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

/*
 * unlink edges in the filesystem subgraph from a starting edge, downwards, transitively
 *
 * fse - edge to begin with
 */
xapi fsedge_disintegrate(fsedge * restrict fse, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull(1)));

/* disconnect a filesystem edge */
xapi fsedge_disconnect(fsedge * restrict fse)
  __attribute__((nonnull));

#endif
