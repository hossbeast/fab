/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

fsedge objects are edges in the graph which represent the filesystem tree

*/

#include "types.h"
#include "valyria/llist.h"
#include "moria.h"

#include "graph.h"

struct fsent;
struct graph_invalidation_context;
struct entlist;

// active list
extern llist fsedge_list;

/* EDGE_FS */
typedef struct fsedge {
  moria_edge edge;
  uint32_t refmark;
} fsedge;

/* release edges */
void fsedge_cleanup(void);

void fsedge_alloc(fsedge ** restrict rv)
  __attribute__((nonnull));

void fsedge_connect(
    struct fsent * restrict B
  , struct fsent * restrict A
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void fsedge_connect_ctx(
    struct fsent * restrict B
  , struct fsent * restrict A
  , struct moria_connect_context * restrict ctx
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void fsedge_release(fsedge * restrict fse)
  __attribute__((nonnull));

void fsedge_disconnecting(fsedge * restrict fse, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
