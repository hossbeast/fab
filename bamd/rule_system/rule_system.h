/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "graph.h"

struct rule_module_association;
struct bind_module_association;
struct rule_run_context;

/* auto-inc reconciliation round */
extern uint32_t rule_system_epoch;

void rule_system_setup(void);

typedef struct rule_system_work {
//  vertex_type mark;
  struct {
    llist lln;                  // rule_system rma_list
    uint32_t epoch;
  } changed[2];
} rule_system_work;

/*
 * run invalidated rules to quiescence
 *
 * reconciled - (returns) false if any rules could not be run
 */
void rule_system_reconcile(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

/*
 * enqueue this rma to be executed during the next reconciliation
 */
void rule_system_work_enqueue(struct rule_system_work * restrict work)
  __attribute__((nonnull));

/*
 * track that a rule was run and had matches
 */
void rule_system_rma_hit(struct rule_run_context * restrict ctx, struct rule_module_association * restrict rma)
  __attribute__((nonnull));

/*
 * track that a rule was run and had no matches
 */
void rule_system_rma_nohit(struct rule_run_context * restrict ctx, struct rule_module_association * restrict rma)
  __attribute__((nonnull));
