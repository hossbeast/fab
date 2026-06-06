/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "moria.h"

struct graph_invalidation_context;
struct rule_module_association;
struct formula;
struct buildplan_entity;
struct module;
enum edge_type;
struct entlist;

/*
meta-edge types
 EDGE_FML_BPE
 EDGE_FML_REC_BPE
 EDGE_FML_REC_BUILD_REC
 EDGE_FML_RMA
 EDGE_FSNAME_MOD
 EDGE_FSNAME_PLUGIN
 EDGE_FSNAME_RMA
 EDGE_LOOKUP
 EDGE_RMA_BIND
 EDGE_RMA_BPE
 EDGE_RMA_MOD
 EDGE_RULE_RMA
 EDGE_RULE_RULESET
*/

typedef struct meta_edge {
  struct moria_edge edge;
  uint16_t epoch;

  /* cached */
  struct rule_module_association *rma;
  struct formula *fml;
  struct buildplan_entity *bpe;
  struct module *mod;
  struct fsent *tgt;
  struct fsname *index;
} meta_edge;

void meta_edge_cleanup(void);

void meta_disconnecting(
    meta_edge * restrict me
  , struct entlist * restrict edges
  , struct entlist * restrict vertices
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void meta_edge_release(meta_edge * restrict se)
  __attribute__((nonnull));

meta_edge *meta_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , enum edge_type type
  , struct graph_invalidation_context * restrict invaliation
)
  __attribute__((nonnull));
