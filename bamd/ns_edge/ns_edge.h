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
 * EDGE_FSNAME_NS aka fsname B -> namespace A
 * EDGE_FSTREE_NS aka fstree B -> namespace A
 */
typedef struct ns_edge {
  struct moria_edge edge;
  uint32_t epoch;
} ns_edge;

extern llist ns_edge_list;

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

void ns_edge_disconnecting(ns_edge * restrict nse, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
