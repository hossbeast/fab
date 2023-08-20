/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "moria.h"

struct set;
struct rule;
struct module;
struct graph_invalidation_context;

/* EDGE_MOD_RULE aka rule A : module B */
typedef struct rule_module_edge {
  moria_edge edge;
  llist lln_rmas_owner;      // in module->rmas_owner
  rbnode nohits_rbn;         // in rule_run_context->nohits
  llist dependencies;        // list of struct dependency created by this rma

  struct module * mod_owner; // module that defines the rule
  struct rule * rule;        // rule to run
  struct module * mod;       // module to run in context of
  struct set * variants;     // variants for rule execution

  struct {
    llist lln;               // rule_system rma_list
    uint32_t reconciliation_id;
  } changed[2];
} rule_module_edge;

/*
 * idempotent create rule <--> module edge
 *
 * rme        - (returns) rme
 * mod        - module to associate
 * mod_owner  - module which defines the rule
 * rule       - rule to associate
 * [variants] - variants for rule execution
 */
void rule_module_connect(
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
void rule_module_disconnect(
    rule_module_edge * restrict rma
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull));

void rule_module_edge_alloc(rule_module_edge ** restrict rmep)
  __attribute__((nonnull));

void rule_module_cleanup(void);
