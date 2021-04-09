/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FABD_RULE_H
#define FABD_RULE_H

/*

A rule is connected to each directory node where its match pattern is run with a EGE_RULE_DIR edge

A rule is connected to each formula node which its formula pattern resolves to with a EDGE_RULE_FML edge

A rule is connected to each module for which it is run with an EDGE_MOD_RULE edge

*/

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"
#include "moria.h"

#include "graph.h"

struct attrs16;
struct module;
struct moria_graph;
struct narrator;
struct pattern;
struct rbtree;
struct rule_module_edge;
struct set;

#define RULE_DIRECTION_OPT    0x0003
#define RULE_CARDINALITY_OPT  0x003c

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
  DEF(RULE_ZERO_TO_ONE  , 0x0004)     /*     --     */  \
                                      /*     -- [*] */  \
  DEF(RULE_ZERO_TO_MANY , 0x0008)     /* [*] --     */  \
                                      /*     <-     */  \
  DEF(RULE_ONE_TO_ONE   , 0x000c)     /*     ->     */  \
                                      /*     <- [*] */  \
  DEF(RULE_ONE_TO_MANY  , 0x0010)     /*     -> [*] */  \
                                      /* [*] <-     */  \
  DEF(RULE_MANY_TO_ONE  , 0x0014)     /* [*] ->     */  \
                                      /* [*] <- [*] */  \
  DEF(RULE_MANY_TO_MANY , 0x0018)     /* [*] -> [*] */  \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum rule_cardinality {
RULE_CARDINALITY_TABLE
} rule_cardinality;

extern struct attrs16 * rule_cardinality_attrs;
extern llist rule_list;     // active
extern llist rde_list;      // active

// VERTEX_TYPE_RULE
typedef struct rule {
  moria_vertex vertex;

  struct pattern * match;
  struct pattern * generate;
  struct pattern * formula;

  edge_type relation;
  rule_direction dir;
  rule_cardinality card;

  llist lln;                // in module statement_block->rules
} rule;

/* EDGE_RULE_DIR : edge connecting a rule to a directory fsent */
typedef struct rule_dirnode_edge {
  moria_edge edge;
  struct rule_module_edge *rme;
} rule_dirnode_edge;

typedef struct rule_run_context {
  struct rule_module_edge *rme;         // rule-module-association
  struct module * mod;                  // module the rule is associated to
  struct module * mod_owner;            // module which defines the rule
  const struct llist * modules;
  const struct set * variants;

  /* scratch space */
  struct set * match_nodes;
  struct set * generate_nodes;

  struct moria_vertex **Alist;
  size_t Alist_alloc;
  struct moria_vertex **Blist;
  size_t Blist_alloc;
  struct pattern_match_node **Mlist;
  size_t Mlist_alloc;

  /* invalidation context for the ongoing operation */
  struct graph_invalidation_context invalidation;

  /* tracking rule/module associations with no matches */
  struct rbtree *nohits;
  bool *reconciled;
} rule_run_context;

xapi rule_run_context_xinit(rule_run_context * rule_ctx)
  __attribute__((nonnull));

xapi rule_run_context_xdestroy(rule_run_context * rule_ctx)
  __attribute__((nonnull));

/*
 * Run a rule to add nodes and edges to the graph.
 */
xapi rule_run(rule * restrict rule, rule_run_context * restrict ctx)
  __attribute__((nonnull));

xapi rule_mk(
    rule ** restrict rulep
  , struct moria_graph * restrict g
  , struct pattern * match
  , struct pattern * generate
  , struct pattern * formula
  , edge_type relation
  , uint32_t attrs
)
  __attribute__((nonnull(1, 2)));

void rule_release(rule * restrict)
  __attribute__((nonnull));

xapi rule_say(const rule * restrict r, struct narrator * restrict N)
  __attribute__((nonnull));

/* release rules */
void rule_cleanup(void);

#endif
