/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria.h"

#include "graph.h"

struct graph_invalidation_context;
struct module;
struct moria_vertex;
enum edge_type;

extern llist module_edge_list; // active list

/*
 * EDGE_RULESET_BUILD_MOD
 * EDGE_RULESET_ANCHOR_MOD
 * EDGE_RULESET_USES_MOD
 * EDGE_VARSET_MOD
 */
typedef struct module_edge {
  struct moria_edge edge;

  /* module-reconcile epoch where this edge was last renewed */
  uint16_t epoch;
} module_edge;

void module_module_cleanup(void);

void module_edge_disconnecting(
    module_edge * restrict me
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

bool module_edge_connect(
    struct moria_vertex * restrict B
  , struct module * restrict A
  , enum edge_type type
  , struct module_edge ** restrict mmep
)
  __attribute__((nonnull(1, 2)));

void module_edge_release(module_edge * restrict me)
  __attribute__((nonnull));

void module_edges_show(void);
