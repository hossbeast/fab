/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria.h"
#include "descriptor.h"

#include "stats.h"
#include "vertices.h"
#include "graph.h"

struct narrator;
struct fsent;
struct graph_invalidation_context;
struct vertices;
struct edges;
struct rule_run_context;
struct formula;
struct bacon_writer;
struct formula_record;
struct build_record;
struct graph_disintegrate_context;
struct identity_ctx;

// active list
extern llist bpe_list;

typedef union stats_bpe {
  struct {
    uint16_t build_err;
    uint16_t build_ok;
  };

  stats_record record;
} stats_bpe;

extern descriptor_type descriptor_stats_bpe;

/*
 * VERTEX_BPE - association between sources (down) and targets (up), and optionally fml (down)
 */
typedef struct buildplan_entity {
  moria_vertex vertex;

  char id[40];
  uint16_t id_len;
  stats_bpe bpe_stats;

  /* shadow link */
  struct fsent *shadow;                 //bpes/id/

  /* cached from edges */
  struct rule_module_association *rma;  // (down) (not-null) rma which created this edge
  struct meta_edge *rma_bpe_me;         // (down) (not-null) rma -> bpe edge
  struct formula *fml;                  // (down) (nullable) associated fml
} buildplan_entity;

/* release buildplan_entity edges */
void buildplan_entity_cleanup(void);
void buildplan_entity_setup(void);

struct buildplan_entity_search_context {
  hashtable_search_context htx;

  /* key */
  const struct rule_module_association *rma;
  struct formula *fml;
  struct moria_vertex **Blist;
  uint16_t Blen;
  struct moria_vertex **Alist;
  uint16_t Alen;
  char id[40];
  uint16_t id_len;
};

buildplan_entity *buildplan_entity_cache_search(struct buildplan_entity_search_context * restrict ctx)
  __attribute__((nonnull));

void buildplan_entity_cache_put(struct buildplan_entity_search_context * restrict ctx, buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void bpe_release(buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void buildplan_entity_alloc(buildplan_entity ** restrict rv)
  __attribute__((nonnull));

/*
 * create a buildplan_entity edge between zero or more nodes
 *
 * Blist - sources
 * Alist - targets
 */
buildplan_entity *buildplan_entity_create_hyper(
    /* 1 */ struct moria_vertex ** restrict Blist
  , /* 2 */ uint16_t Blen
  , /* 3 */ struct moria_vertex ** restrict Alist
  , /* 4 */ uint16_t Alen
  , /* 5 */ struct formula * restrict fml
  , /* 6 */ struct rule_module_association * restrict rma
  , /* 7 */ struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(6, 7)));

void buildplan_entity_source_edges(const buildplan_entity * restrict bpe, struct edges * restrict list)
  __attribute__((nonnull));

/* get a list of the buildplan_entity sources */
void buildplan_entity_sources(const buildplan_entity * restrict bpe, struct vertices * restrict list)
  __attribute__((nonnull));

void buildplan_entity_target_edges(const buildplan_entity * restrict bpe, struct edges * restrict list)
  __attribute__((nonnull));

/* get a list of the buildplan_entity targets */
void buildplan_entity_targets(const buildplan_entity * restrict bpe, struct vertices * restrict list)
  __attribute__((nonnull));

void buildplan_entity_say_sources(const buildplan_entity * restrict bpe, struct narrator * restrict N)
  __attribute__((nonnull));

void buildplan_entity_say_targets(const buildplan_entity * restrict bpe, struct narrator * restrict N)
  __attribute__((nonnull));

size_t buildplan_entity_targets_cat(void * restrict dst, size_t sz, const buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void bpe_system_reconcile(struct graph_invalidation_context * restrict ctx)
  __attribute__((nonnull));

void buildplan_entity_validate_enqueue(buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void bpe_describe(const buildplan_entity * restrict bpe, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void bpe_identity_describe(const buildplan_entity * restrict bpe, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t bpe_path_cat(void * restrict dst, size_t sz, const buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void bpe_stats_build(buildplan_entity * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void bpe_link_record(buildplan_entity * restrict bpe, struct build_record * restrict build_rec, struct formula_record * restrict rec)
  __attribute__((nonnull));

void bpe_unlink(buildplan_entity * restrict bpe, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

bool bpe_reap(struct buildplan_entity * restrict bpe, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void bpe_hash(const buildplan_entity * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void bpe_id(const buildplan_entity * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void bpe_compute_id(const buildplan_entity * restrict bpe, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void bpes_show(void);
