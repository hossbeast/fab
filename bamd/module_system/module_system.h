/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/llist.h"

struct rule_run_context;
struct config;
struct graph_invalidation_context;
struct rbtree;

extern llist module_rebuilds;   // invalidated modules ; to rebuild
extern llist ruleset_rebuilds;  // invalidated rulesets ; to rebuild

extern struct rbtree *modules_varsets;
extern struct rbtree *modules_rulesets;

/*
 * reload invalidated modules
 */
void module_system_reconcile(struct rule_run_context * restrict rule_ctx)
  __attribute__((nonnull));

void modules_invalidate_primary(struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void module_system_reconfigure(struct config *cfg, bool dryrun, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
