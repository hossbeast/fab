/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"
#include "valyria/llist.h"

struct graph_invalidation_context;
enum edge_type;

/*
 * EDGE_DIR_FSNAME aka dirnode B -> fsname A
 *
 * These edges are created for dirnodes in filesystems that are indexed for lookup.
 * When the set of directory nodes attached to the index-vertex changes, that
 * invalidation is propagated upwards to rmas (whose search matched that index-vertex)
 * and to modules (which had referenced that index-vertex for imports).
 */
typedef struct dir_fsname_edge {
  struct moria_edge edge;
  uint32_t epoch;
} dir_fsname_edge;

extern llist dir_fsname_edge_list;

void dir_fsname_edge_setup(void);
void dir_fsname_edge_cleanup(void);

void dir_fsname_edge_release(dir_fsname_edge * restrict se)
  __attribute__((nonnull));

dir_fsname_edge *dir_fsname_edge_alloc(void);

void dir_fsname_edge_disconnecting(dir_fsname_edge * restrict nse, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void dir_fsname_edges_show(void);

void dir_fsname_edge_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
