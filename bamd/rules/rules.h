/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*


*/

#include "types.h"
#include "valyria/llist.h"
#include "bam/stats.h"

#include "graph.h"
#include "module_system.h"

struct fsent;
struct graph_invalidation_context;

extern llist rules_list;    // active rules files

/* VERTEX_RULES */
typedef struct rules {
  moria_vertex vertex;

  char id[40];
  uint16_t id_len;
  char token[40];
  uint16_t token_len;

  /* adhoc */
  llist lln;
  llist rmas_owner;         // rmas for rules in this modules statement blocks
  rules_container invalid;  // in rebuild lists

  /* rules */
  llist blocks;              // struct block

  bam_vertex_stats stats;          // stats for rules vertex itself
  bam_rules_stats rules_stats;     // stats for rules.bam
  struct fsent * self_node;        // rules.bam
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;
} rules;

void rules_setup(void);
void rules_cleanup(void);

void rules_alloc(rules ** restrict rp);

void rules_dispose(rules * restrict)
  __attribute__((nonnull));

void rules_reparse(rules * restrict)
  __attribute__((nonnull));

void rules_release(rules * restrict, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void rules_bootstrap(struct fsent * restrict rules_file_n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void rules_rebuild_enqueue(rules * restrict rules)
  __attribute__((nonnull));

void rules_excise(rules * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void rules_disconnect(rules * restrict mod, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

size_t rules_stats_collate(void *dst, size_t sz, bool reset, rules * restrict)
  __attribute__((nonnull));
