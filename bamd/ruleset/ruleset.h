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

extern llist ruleset_list;    // active ruleset files

/* BAM_FSENT_TYPE_RULESET_FILE - rules.bam file */
typedef union stats_ruleset {
  struct {
    /* event counters */
    uint16_t parse;         /* parse attempted */
    uint16_t parse_ok;      /* parsed successfully */

    /* entity counters */
    uint32_t rules;         /* number of rules in this file */
    uint32_t uses_up;       /* count of modules using this ruleset file */
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
  rules_container invalid;  // in rebuild lists

  /* rules */
  llist blocks;              // struct block

  stats_vertex stats;          // stats for rules vertex itself
  stats_ruleset ruleset_stats; // stats for rules.bam
  struct fsent * self_node;        // rules.bam
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;
} ruleset;

void ruleset_setup(void);
void ruleset_cleanup(void);

void ruleset_alloc(ruleset ** restrict rp);

void ruleset_dispose(ruleset * restrict)
  __attribute__((nonnull));

void ruleset_reparse(ruleset * restrict)
  __attribute__((nonnull));

void ruleset_release(ruleset * restrict, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void ruleset_bootstrap(struct fsent * restrict ruleset_file_n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void ruleset_rebuild_enqueue(ruleset * restrict ruleset)
  __attribute__((nonnull));

void ruleset_excise(ruleset * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void ruleset_disconnect(ruleset * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void ruleset_describe(const ruleset * restrict ruleset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void ruleset_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t ruleset_path_cat(void * restrict dst, size_t sz, const ruleset * restrict ruleset)
  __attribute__((nonnull));

void ruleset_identity_say(const struct ruleset * restrict ruleset, struct narrator * restrict N)
  __attribute__((nonnull));

void ruleset_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
  __attribute__((nonnull));

void ruleset_stats_build(ruleset * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void ruleset_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));
