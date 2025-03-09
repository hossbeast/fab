/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "moria.h"
#include "rule_system.h"
#include "vertex.h"
#include "descriptor.h"
#include "stats.h"
#include "types.h"
#include "lookup.h"

struct hashtable;
struct rule;
struct module;
struct graph_invalidation_context;
struct fsedge;
struct bacon_writer;
struct narrator;
struct graph_disintegrate_context;
struct ruleset;
struct rule_module_association;

extern llist rma_list;  // active

/* BAM_FSENT_TYPE_SHADOW_RULE //modules/id/rules/id - this represents a rule/module association */
typedef union stats_rma {
  struct {
      /* event counters */
      uint32_t rule_resolve_ok;   /* rule run successfully */
      uint32_t rule_resolve_err;  /* rule run and error encountered */
      uint32_t rule_run;          /* rule is run */

      /* edge counts */
      uint16_t bpes;              /* rma-bpe edges */
      uint16_t dirs;              /* dir-rma edges */

      /* results of most recent run */
      uint16_t target_hits;
      uint16_t attachment_hits;
      uint16_t search_hits;          /* nodes matched */
      uint16_t search_rhs_hits;
      uint16_t generate_hits;
      uint16_t formula_hits;
  };

  stats_record record;
} stats_rma;

extern descriptor_type descriptor_stats_rma;

/*
 * VERTEX_RMA aka rule -> module association
 *
 * These vertices are created when a rule is run in the context of a module.
 */
typedef struct rule_module_association {
  moria_vertex vertex;

  stats_vertex stats;
  stats_rma rma_stats;
  char id[40];                 // hash over rule-id + module-id
  uint16_t id_len;

  /* cached */
  struct formula *fml;         // (EDGE_FML_RMA down)    formula resolved from lookup pattern at rule_resolve time (can be null)
  struct rule *rule;           // (EDGE_RULE_RMA down)   rule to run
  struct module *mod;          // (EDGE_RMA_MOD up)      module to run the rule in context of
  //struct ruleset *rules;       // (EDGE_RMA_RULESET up)  rules file the rule statement is in (can be null)
  struct meta_edge *fml_rma_me;// (down) (nullable)      fml -> rma edge

  /* other */
  struct fsent *dirnode;       // dirnode containing the rules.bam file containing the rule
  lookup_store lookups;

  /* rule_system state */
//  uint16_t reconciliation_epoch;
  rule_system_work work;
} rule_module_association;

void rule_module_association_setup(void);
void rule_module_association_cleanup(void);

/*
 * idempotent create rule <--> module edge
 *
 * rme    - (returns) rme
 * mod    - module to associate
 * rules  - module which defines the rule
 * rule   - rule to associate
 */
void rma_create(
    struct rule * restrict rule
  , struct module * mod
  , struct ruleset * rules
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void rma_release(rule_module_association * restrict rma)
  __attribute__((nonnull));

void rma_unlink(rule_module_association * restrict rma, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void rule_module_association_init(rule_module_association * restrict rma)
  __attribute__((nonnull));

void rma_describe(const rule_module_association * restrict rma, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t rma_path_cat(void * restrict dst, size_t sz, const rule_module_association * restrict rma)
  __attribute__((nonnull));

void rma_stats_build(rule_module_association * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void rma_identity_describe(const rule_module_association * restrict rule, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void rma_system_reconcile(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void rma_resolve(struct rule_module_association * restrict rma)
  __attribute__((nonnull));

void rma_reset(struct rule_module_association * restrict rma)
  __attribute__((nonnull));

void rma_stats_describe(const rule_module_association * restrict rma, struct  bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void rma_stats_say(const rule_module_association * restrict rma, struct narrator * restrict N)
  __attribute__((nonnull));

void rma_stats_show(const rule_module_association * restrict rma)
  __attribute__((nonnull));

void rma_hash(const rule_module_association * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void rma_id(const rule_module_association * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void rma_compute_id(const struct rule_module_association * restrict module, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void rmas_show(void);
