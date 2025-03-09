/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "descriptor.h"
#include "stats.h"
#include "types.h"
#include "valyria/llist.h"

#include "graph.h"
#include "module_system.h"
#include "vertex.h"

struct fsent;
struct graph_invalidation_context;
struct graph_disintegrate_context;
struct lookup_pattern;

extern llist ruleset_list;    // active ruleset files

/* BAM_FSENT_TYPE_RULESET_FILE - rules.bam file */
typedef union stats_ruleset {
  struct {
    /* event counters */
    uint16_t parse_ok;      /* parsed successfully */
    uint16_t parse_err;     /* parse failed */

    /* entity counters */
    uint32_t rules;         /* number of rules in this file */
    uint32_t modules;       /* number of modules using this ruleset file */
  };

  stats_record record;
} stats_ruleset;

extern descriptor_type descriptor_stats_ruleset;

/* VERTEX_RULESET */
typedef struct ruleset {
  moria_vertex vertex;

  char id[40];
  uint16_t id_len;

  /* adhoc */
  llist lln;
  llist rmas_owner;         // rmas for rules in this modules statement blocks
  llist invalid_lln;        // in rebuild lists

  struct lookup_pattern *anchor;   // anchor pattern in build-dir build.bam
  llist varsets_names;             // struct ruleset_special_name
  llist rulesets_names;            // struct ruleset_special_name
  bool managed;

  /* rules */
  llist blocks;                       // struct block

  stats_vertex stats;                 // stats for rules vertex itself
  stats_ruleset ruleset_stats;        // stats for rules.bam
  struct fsent *self_node;            // rules.bam
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;
} ruleset;

typedef struct ruleset_special_name {
  llist lln;
  uint16_t len;
  const char *name;
} ruleset_special_name;

void ruleset_setup(void);
void ruleset_cleanup(void);

void ruleset_alloc(ruleset ** restrict rp);

void ruleset_reparse(ruleset * restrict)
  __attribute__((nonnull));

void ruleset_release(ruleset * restrict)
  __attribute__((nonnull));

ruleset *ruleset_bootstrap(struct fsent * restrict ruleset_file_n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void ruleset_rebuild_enqueue(ruleset * restrict ruleset)
  __attribute__((nonnull));

void ruleset_describe(const ruleset * restrict ruleset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void ruleset_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t ruleset_path_cat(void * restrict dst, size_t sz, const ruleset * restrict ruleset)
  __attribute__((nonnull));

void ruleset_identity_describe(const struct ruleset * restrict ruleset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void ruleset_stats_build(ruleset * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void ruleset_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void ruleset_unlink(struct ruleset * restrict ruleset, struct graph_disintegrate_context * restrict)
  __attribute__((nonnull));

void rulesets_show(void);

bool ruleset_reap(struct ruleset * restrict rules, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void ruleset_hash(const ruleset * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void ruleset_id(const ruleset * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void ruleset_compute_id(const struct ruleset * restrict v, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void rulesets_show(void);

void ruleset_rebuild(ruleset * restrict rules, graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
