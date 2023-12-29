/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

A rule is connected to each directory node where its match pattern is run with an EGE_RULE_DIR edge

A rule is connected to each formula node which its formula pattern resolves to with an EDGE_RULE_FML edge

A rule is connected to each module for which it is run with an EDGE_MOD_RULE edge

*/

#include "types.h"

#include "bam/stats.h"
#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria.h"
#include "yyutil/grammar.h"
#include "bam/stats.h"

#include "graph.h"
#include "entlist.h"

struct attrs16;
struct module;
struct moria_graph;
struct narrator;
struct pattern;
struct yyu_location;
struct fsent;
struct bacon_value;
struct rule_module_association;
struct bind_module_association;

extern llist rule_list;     // active

#define RULE_DIRECTION_OPT    0x000f
#define RULE_CARDINALITY_OPT  0x00f0

#define RULE_DIRECTION_TABLE          \
  DEF(RULE_LTR   , 0x0001)  /* -> */  \
  DEF(RULE_RTL   , 0x0002)  /* <- */  \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum rule_direction {
RULE_DIRECTION_TABLE
} rule_direction;

extern struct attrs16 * rule_direction_attrs;

#define RULE_CARDINALITY_TABLE                          \
  DEF(RULE_ZERO_TO_ONE  , 0x0010)     /*     --     */  \
                                      /*     -- [*] */  \
  DEF(RULE_ZERO_TO_MANY , 0x0020)     /* [*] --     */  \
                                      /*     <-     */  \
  DEF(RULE_ONE_TO_ONE   , 0x0030)     /*     ->     */  \
                                      /*     <- [*] */  \
  DEF(RULE_ONE_TO_MANY  , 0x0040)     /*     -> [*] */  \
                                      /* [*] <-     */  \
  DEF(RULE_MANY_TO_ONE  , 0x0050)     /* [*] ->     */  \
                                      /* [*] <- [*] */  \
  DEF(RULE_MANY_TO_MANY , 0x0060)     /* [*] -> [*] */  \
  DEF(RULE_BIND_ONE     , 0x0070)     /*     =>     */  \
  DEF(RULE_BIND_MANY    , 0x0080)     /*     => [*] */  \
  DEF(RULE_SEARCH_SEARCH, 0x0090)     /*     ->     */  \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum rule_cardinality {
RULE_CARDINALITY_TABLE
} rule_cardinality;

extern struct attrs16 * rule_cardinality_attrs;

// VERTEX_TYPE_RULE
typedef struct rule {
  moria_vertex vertex;

  /* rule search -> generate : formula */
  struct pattern *search;
  struct pattern *generate;
  struct pattern *formula;

  /* bind lookup -> attachment */
  struct pattern *target;
  struct pattern *attachment;

  /* search -> search */
  struct pattern *search_rhs;

  edge_type graph;
  bool bind;
  rule_direction dir;
  rule_cardinality card;
  bool anchor_search;
  bool module_search;

  yyu_location loc;
  bam_vertex_stats stats;
  char id[40];
  uint16_t id_len;
  char token[40];
  uint16_t token_len;

  llist lln;                // in module statement_block->rules
} rule;

typedef struct rule_run_context {
  struct rule_module_association *rma;
  struct bind_module_association *bma;

//  bam_rule_stats stats;

  /* scratch space */
  struct hashtable search_hits;     // search pattern results
  struct entlist generate_hits;     // generate pattern results
  struct entlist target_hits;       // target pattern results
  struct entlist target_index_hits; // index-vertex lookup nodes visited by the target pattern
  struct hashtable search_rhs_hits; // rhs search pattern results

  /* scratch node lists used in rule_run */
  struct moria_vertex **Blist;
  size_t Blist_alloc;
  struct moria_vertex **Alist;
  size_t Alist_alloc;
  struct pattern_search_node **Mlist;
  size_t Mlist_alloc;

  /* the matched formula, if any */
  struct formula *fml;

  /* invalidation context for the ongoing operation */
  struct graph_invalidation_context invalidation;
} rule_run_context;

void rule_run_context_init(rule_run_context * rule_ctx)
  __attribute__((nonnull));

void rule_run_context_destroy(rule_run_context * rule_ctx)
  __attribute__((nonnull));

/*
 * Run a rule to add nodes and edges to the graph.
 */
void rule_run(rule * restrict rule, rule_run_context * restrict ctx)
  __attribute__((nonnull));

typedef struct rule_bacon {
  edge_kind graph;
  char id[32];
  uint16_t id_len;
  bool anchor_search;
  bool module_search;
} rule_bacon;

void rule_mk(
    rule ** restrict rulep
  , struct moria_graph * restrict g
  , struct pattern * restrict search
  , struct pattern * restrict generate
  , struct pattern * restrict formula
  , struct pattern * restrict target
  , struct pattern * restrict attachment
  , struct pattern * restrict search2
  , const rule_bacon * restrict bacon
  , uint32_t attrs
  , const struct yyu_location * restrict loc
)
  __attribute__((nonnull(1, 2)));

/* destroy a rule and return it to the freelist. The rule must have previously been
 * excised from the graph */
void rule_release(rule * restrict)
  __attribute__((nonnull));

/* destroy a rule and return it to the freelist */
void rule_dispose(rule * restrict)
  __attribute__((nonnull));

void rule_say(const rule * restrict r, struct narrator * restrict N)
  __attribute__((nonnull));

void rule_show(const rule * restrict r)
  __attribute__((nonnull));

/* release rules */
void rule_cleanup(void);

void rule_describe(const rule * restrict r, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

int rule_validate(const rule * restrict rule, char * restrict error, size_t error_sz)
  __attribute__((nonnull));
