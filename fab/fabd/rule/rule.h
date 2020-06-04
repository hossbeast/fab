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

Rules are attached to one or more directory nodes, based on what nodes their match pattern could
possibly match.



*/

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"

#include "graph.h"

struct attrs16;
struct artifact;
struct narrator;
struct list;
struct map;
struct node;
struct pattern;
struct variant;
struct set;
struct module;
struct graph;
struct edge_traversal_state;
struct rule_module_association;

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
  DEF(RULE_ZERO_TO_MANY , 0x0008)     /* [*] -- [*] */  \
  DEF(RULE_ONE_TO_ONE   , 0x000c)     /*     ->     */  \
  DEF(RULE_ONE_TO_MANY  , 0x0010)     /*     -> [*] */  \
  DEF(RULE_MANY_TO_ONE  , 0x0014)     /* [*] ->     */  \
  DEF(RULE_MANY_TO_MANY , 0x0018)     /* [*] -> [*] */  \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum rule_cardinality {
RULE_CARDINALITY_TABLE
} rule_cardinality;

extern struct attrs16 * rule_cardinality_attrs;

// allocated as the value of a vertex
typedef struct rule {
  struct pattern * match;
  struct pattern * generate;
  struct pattern * formula;
  struct node * fml_node;   // lookup from formula

  edge_type relation;
  rule_direction dir;
  rule_cardinality card;

  llist lln;                // in block->rules
} rule;

STATIC_ASSERT(sizeof(rule) <= GRAPH_VERTEX_VALUE_SIZE);

/* edge connecting a rule to a directory node */
typedef struct rule_dirnode_edge {
  struct rule_module_association *rma;
} rule_dirnode_edge;

STATIC_ASSERT(sizeof(rule_dirnode_edge) <= GRAPH_EDGE_VALUE_SIZE);

typedef struct rule_run_context {
  struct module * mod;            // module the rule is associated to
  struct module * mod_owner;      // module which defines the rule
  const struct llist * modules;
  const struct set * variants;
  struct graph_invalidation_context invalidation;

  /* outputs */
  struct set * match_nodes;
  struct set * generate_nodes;

  /* multi-rule run situation */
  struct rule_module_association *rma;

  /* rule/module associations with no matches */
  rbtree nohits;
} rule_run_context;

xapi rule_run_context_xinit(rule_run_context * rule_ctx)
  __attribute__((nonnull));
xapi rule_run_context_begin(rule_run_context * restrict rule_ctx)
  __attribute__((nonnull));
void rule_run_context_end(rule_run_context * rule_ctx)
  __attribute__((nonnull));
xapi rule_run_context_xdestroy(rule_run_context * rule_ctx)
  __attribute__((nonnull));

/// rule_run
//
// SUMMARY
//  Apply all invalidated rules to add nodes and edges to the graph.
//
//  Rules are applied iteratively, starting with
//  the artifact, and proceeding to its consequents, to their consequents, and so on.
//
// PARAMETERS
//  rule           -
//  module         -
//  modules        - all modules
//  variants       - variants the rule applies to (not all variants)
//  match_nodes    - work space
//  generate_nodes - work space
//
xapi rule_run(rule * restrict rule, rule_run_context * restrict ctx)
  __attribute__((nonnull));

xapi rule_mk(
    rule ** restrict rulep
  , struct graph * restrict g
  , struct pattern * match
  , struct pattern * generate
  , struct pattern * formula
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

xapi rule_xdestroy(rule * restrict r)
  __attribute__((nonnull));

xapi rule_say(const rule * restrict r, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
