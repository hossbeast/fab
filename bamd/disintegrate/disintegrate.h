/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct moria_vertex;
struct edges;
struct vertices;
struct graph_invalidation_context;

typedef struct graph_disintegrate_context {
  struct graph_invalidation_context * restrict invalidation;
  struct edges * restrict edges;
  struct vertices * restrict unlinking;
  struct vertices * restrict releasing;
} graph_disintegrate_context;

/*
 * recursively cleanup vertices and edges
 *
 * this algorithm has three working lists and it proceeds until all of them are empty
 *
 * unlinking - a vertex on the unlinking list, if not already unlinked, is unlinked and moved to the releasing
 *             list. when a vertex is unlinked, its edges with EDGE_UNLINK are added to the edges list
 *
 * releasing - a vertex on the releasing list is released if it has no edges
 *
 * edges - an edge on the edges list is released, and each of its vertices is added
 *         to the releasing list
 *
 * when a vertex is unlinked, its unlink handler may add some of its edges to the
 * edges list, and some of its connected vertices to the unlinking or releasing lists
 *
 * when a vertex is on the releasing list, if it is not unlinked, it may nevertheless
 * be unlinked if the reap handler for that vertex type returns true
 */
void graph_disintegrate(
    struct edges * restrict edges
  , struct vertices * restrict unlinking
  , struct vertices * restrict releasing
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1)));

/* disintegrate starting from a single vertex */
void graph_demolish(struct moria_vertex * restrict v)
  __attribute__((nonnull));
