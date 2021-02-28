/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_RULE_MODULE_H
#define FABD_RULE_MODULE_H

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "moria.h"

#include "graph.h"

struct set;
struct rule;
struct module;

/* EDGE_RULE_MOD aka rule A : module B */
typedef struct rule_module_edge {
  moria_edge edge;
  llist lln_rmas_owner;      // in module->rmas_owner
  llist lln_refresh;
  rbnode nohits_rbn;         // in rule_run_context->nohits

  llist dependencies;        // struct dependency - created by this rma

  struct module * mod_owner; // module which defines the rule
  struct rule * rule;        // rule to run
  struct module * mod;       // module to run in context of (associated module)
  struct set * variants;     // variants for rule execution

  struct {
    llist lln;               // in module_rma_list
    uint32_t reconciliation_id;
  } changed[2];
} rule_module_edge;

/*
 * idempotent create rule <--> module edge
 *
 * rmep      - (returns) rme
 * mod       - module to associate
 * mod_owner - module which defines the rule
 * rule      - rule to associate
 * variants  - variants for rule execution
 */
xapi rule_module_connect(
    rule_module_edge ** restrict rma
  , struct module * mod
  , struct module * mod_owner
  , struct rule * restrict rule
  , struct set * restrict variants
)
  __attribute__((nonnull(1, 2, 3, 4)));

/*
 *
 */
xapi rule_module_disconnect(
    rule_module_edge * restrict rma
  , graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

xapi rule_module_edge_alloc(rule_module_edge ** restrict rmep)
  __attribute__((nonnull));

xapi rule_module_xdestroy(rule_module_edge * restrict rme)
  __attribute__((nonnull));

xapi rule_module_cleanup(void);

#endif
