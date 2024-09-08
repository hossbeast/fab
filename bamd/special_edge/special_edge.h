/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"

struct graph_invalidation_context;
struct entlist;
enum edge_type;

/*
 * A special edge connects an fsent vertex B -> special vertex A
 */
typedef struct special_edge {
  struct moria_edge edge;
} special_edge;

void special_edge_cleanup(void);

void special_disconnecting(
    special_edge * restrict me
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void special_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , struct graph_invalidation_context * restrict invaliation
)
  __attribute__((nonnull));

void special_connect_type(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , enum edge_type type
  , struct graph_invalidation_context * restrict invaliation
)
  __attribute__((nonnull));

void special_edge_release(special_edge * restrict se)
  __attribute__((nonnull));
