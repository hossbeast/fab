/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"
#include "valyria/llist.h"

struct graph_invalidation_context;
struct entlist;
enum edge_type;

/*
 * EDGE_SHADOW_REF aka vertex B -> shadow-link A
 */
typedef struct ref_edge {
  struct moria_edge edge;
} ref_edge;

extern llist ref_edge_list;

void ref_edge_release(ref_edge * restrict se)
  __attribute__((nonnull));

ref_edge *ref_edge_alloc(void);

void ref_edge_setup(void);
void ref_edge_cleanup(void);

ref_edge *ref_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , enum edge_type type
)
  __attribute__((nonnull));

void ref_disconnecting(
    ref_edge * restrict ref
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void ref_edges_show(void);
