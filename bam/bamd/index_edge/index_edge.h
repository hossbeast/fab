/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"

struct graph_invalidation_context;
struct entlist;
enum edge_type;

typedef struct index_edge {
  struct moria_edge edge;
} index_edge;

void index_edge_cleanup(void);

void index_disconnecting(
    index_edge * restrict me
  , struct entlist * restrict edges
  , struct entlist * restrict vertices
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void index_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , enum edge_type type
  , struct graph_invalidation_context * restrict invaliation
)
  __attribute__((nonnull));

void index_edge_release(index_edge * restrict se)
  __attribute__((nonnull));

void index_edge_alloc(index_edge ** restrict rv)
  __attribute__((nonnull));

void index_edge_setup(void);
void index_edge_cleanup(void);
