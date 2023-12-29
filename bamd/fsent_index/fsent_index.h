/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct fsent;
struct bacon_writer;
struct index_vertex;
struct graph_invalidation_context;

extern uint32_t fsent_index_count;
extern struct index_vertex *g_index;

/* index a non-shadow directory node for graph lookup by label */
void fsent_index_put(struct fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

/* remove a previously indexed node from the graph lookup data structure */
void fsent_index_del(struct fsent * restrict n, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

void fsent_index_show(void);
void fsent_index_describe(struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void fsent_index_setup(void);
void fsent_index_cleanup(void);
