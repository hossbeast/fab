/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"

struct graph_invalidation_context;
struct entlist;
enum edge_type;

/*
 * EDGE_FSNAME_FSINDEX aka fsname B -> namespace B
 * EDGE_DIR_FSNAME aka dirnode B -> fsname A
 *
 * These edges are created for dirnodes in filesystems that are indexed for lookup.
 * When the set of directory nodes attached to the index-vertex changes, that
 * invalidation is propagated upwards to rmas (whose search matched that index-vertex)
 * and to modules (which had referenced that index-vertex for imports).
 */
typedef struct ns_edge {
  struct moria_edge edge;
  uint32_t epoch;
} ns_edge;

void ns_edge_release(ns_edge * restrict se)
  __attribute__((nonnull));

ns_edge *ns_edge_alloc(void);

void ns_edge_setup(void);
void ns_edge_cleanup(void);

ns_edge *ns_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , enum edge_type type
)
  __attribute__((nonnull));
