/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

A rule is connected to each directory node where its match pattern is run with an EGE_RULE_DIR edge

A rule is connected to each formula node which its formula pattern resolves to with an EDGE_RULE_FML edge

A rule is connected to each module for which it is run with an EDGE_MOD_RULE edge

*/

#include "types.h"

#include "valyria/llist.h"
#include "valyria/hashtable.h"
#include "moria.h"
#include "yyutil/grammar.h"

#include "graph.h"
#include "edges.h"
#include "vertices.h"
#include "vertex.h"
#include "namespace.h"
#include "lookup.h"
#include "resolve.h"

struct attrs16;
struct module;
struct moria_graph;
struct narrator;
struct pattern;
struct yyu_location;
struct fsent;
struct bacon_value;
struct rule_module_association;
struct ruleset;
struct request_handler;
struct search_pattern;
struct generate_pattern;
struct attachment_pattern;
struct lookup_pattern;

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

typedef union stats_rule {
  struct {
      uint32_t rule_resolve_ok;   /* rule */
      uint32_t rule_resolve_err;  /* rule */
      uint32_t rule_run;          /* rule is run */
  };

  stats_record record;
} stats_rule;

extern descriptor_type descriptor_stats_rule;

// VERTEX_TYPE_RULE
typedef struct rule {
  moria_vertex vertex;
  llist lln;                // in block->rules

  stats_vertex stats;
  stats_rule rule_stats;
  char id[40];
  uint16_t id_len;
  yyu_location loc;
  const struct ruleset *container;

  char name[255];           // optional name
  uint16_t name_len;

  char namespace[255];      // optional namespace, for bind rules
  uint16_t namespace_len;

  /* rule search -> generate : formula */
  struct search_pattern *search;
  struct generate_pattern *generate;
  struct lookup_pattern *formula;

  /* bind lookup -> attachment */
  struct lookup_pattern *target;
  struct attachment_pattern *attachment;

  /* search -> search */
  struct search_pattern *search_rhs;

  rule_direction dir;
  rule_cardinality card;
} rule;

typedef struct rma_operation {
  llist lln;
  struct timespec start_time;
  struct timespec end_time;
  int level;
} rma_operation;

typedef struct rule_run_context {
  struct rule_module_association *rma;

  /* operation state tracking */
  llist operations;                 // struct rma_operation
  int level;

  /* scratch space */
  struct hashtable search_hits;     // search pattern results
  struct vertices generate_hits;    // generate pattern results
  struct hashtable search_rhs_hits; // rhs search pattern results
  struct lookup_context lctx;

  /* scratch node lists used in rule_run */
  struct moria_vertex **Blist;
  size_t Blist_alloc;
  struct moria_vertex **Alist;
  size_t Alist_alloc;
  struct pattern_search_hit **Mlist;
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

/* run a rule : evaluate the left-hand side, right-hand side and create
 * or refresh bpes from the matches */
void rule_run(rule * restrict rule, rule_run_context * restrict ctx)
  __attribute__((nonnull));

/* resolve a rule : resolve references to formulas, and, for bind rules,
 * the attachment and targets */
void rule_resolve(rule * restrict rule, rule_run_context * restrict ctx)
  __attribute__((nonnull));

typedef struct rule_bacon {
  char name[255];
  uint16_t name_len;

  char namespace[255];
  uint16_t namespace_len;
} rule_bacon;

rule *rule_mk(
    struct moria_graph * restrict g
  , const struct ruleset * restrict container
  , struct search_pattern * restrict search
  , struct generate_pattern * restrict generate
  , struct lookup_pattern * restrict formula
  , struct lookup_pattern * restrict target
  , struct attachment_pattern * restrict attachment
  , struct search_pattern * restrict search2
  , const rule_bacon * restrict bacon
  , uint32_t attrs
  , const struct yyu_location * restrict loc
)
  __attribute__((nonnull(1)));

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

int rule_validate(const rule * restrict rule, char * restrict error, size_t error_sz)
  __attribute__((nonnull));

void rule_describe(const rule * restrict r, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void rule_identity_describe(const rule * restrict r, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t rule_path_cat(void * restrict dst, size_t sz, const struct rule * restrict rule)
  __attribute__((nonnull));

void rule_stats_build(rule * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

bool rule_reap(struct rule * restrict rule, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void rule_id(const rule * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void rule_compute_id(const rule * restrict rule, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void rules_show(void);

void rule_unlink(rule * restrict rma, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void rule_tell(const rule * restrict rule, struct request_handler * restrict handler)
  __attribute__((nonnull));
