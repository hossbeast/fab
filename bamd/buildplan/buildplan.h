/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

The buildplan represents a set of formulas to be executed in order to satisfy the current goals.

*/

#include "descriptor.h"
#include "moria/traverse.h"

#include "selection.h"
#include "stats.h"

struct dependency;
struct moria_vertex;
struct llist;
struct bacon_writer;
struct moria_vertex_traversal_state;
struct channel;

enum buildplan_state {
    NOOP          // nothing to do
  , UNSATISFIED   // cant build, unsatisfied dependencies
  , READY         // DEW IT
};

typedef union stats_buildplan {
  struct {
      uint32_t count;
  };

  stats_record record;
} stats_buildplan;

extern descriptor_type descriptor_stats_buildplan;

extern selection buildplan_selection;
extern uint16_t buildplan_id;
extern enum buildplan_state buildplan_state;
extern struct moria_vertex *g_buildplan;

void buildplan_create(void);
void buildplan_setup(void);
void buildplan_cleanup(void);
void buildplan_reset(void);
void buildplan_show(void);

void buildplan_finalize(void)
  __attribute__((nonnull));

/*
 * Add each node in the selection to the plan, along with its dependencies, recursively
 */
void buildplan_select_transitive(struct llist * restrict selection)
  __attribute__((nonnull));

/*
 * add exactly those nodes in the selection to the plan
 */
void buildplan_select_direct(struct llist * restrict selection)
  __attribute__((nonnull));

/*
 * Add a node to the plan along with its transitive dependencies
 */
void buildplan_add_transitive(struct moria_vertex * restrict v, struct moria_vertex_traversal_state * restrict state)
  __attribute__((nonnull(1)));

void buildplan_describe(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void buildplan_tell(struct channel * restrict channel)
  __attribute__((nonnull));

void buildplan_stats_build(bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

size_t buildplan_id_cat(char *dst, size_t sz)
  __attribute__((nonnull));
