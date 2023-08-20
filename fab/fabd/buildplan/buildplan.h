/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

The buildplan represents a set of formulas to be executed in order to satisfy the current goals.

*/

#include "selection.h"

#include "moria/traverse.h"

struct dependency;
struct moria_vertex;
struct llist;

enum buildplan_state {
    NOOP          // nothing to do
  , UNSATISFIED   // cant build, unsatisfied dependencies
  , READY         // DEW IT
};

extern selection buildplan_selection;
extern uint16_t buildplan_id;
extern enum buildplan_state buildplan_state;

void buildplan_create(void);
void buildplan_setup(void);
void buildplan_cleanup(void);
void buildplan_reset(void);
void buildplan_report(void);

/* add a dependency edge which has a formula to the current buildplan */
void buildplan_add(struct dependency * restrict bpe, int distance)
  __attribute__((nonnull));

void buildplan_finalize(void)
  __attribute__((nonnull));

/*
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
void buildplan_visitor_direct(struct moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * restrict result)
  __attribute__((nonnull(1)));

/*
 * add each node in the selection to the plan, along with its dependencies, recursively
 */
void buildplan_select_transitive(struct llist * restrict selection)
  __attribute__((nonnull));

/*
 * add exactly those nodes in the selection to the plan
 */
void buildplan_select_direct(struct llist * restrict selection)
  __attribute__((nonnull));
