/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct narrator;
struct fsent;
struct llist;

/* function pointers for moria/operations which operate on g_graph */
extern struct moria_operations_vtable * node_operations_test_dispatch;

void resolve_fragment(const char *frag, uint16_t fragl, struct fsent **entp)
  __attribute__((nonnull));

void graph_say(struct narrator * restrict N)
  __attribute__((nonnull));

void graph_say_lists(
    struct narrator * restrict N
  , struct llist ** restrict vertex_lists
  , uint16_t vertex_lists_len
  , struct llist ** restrict edge_lists
  , uint16_t edge_lists_len
)
  __attribute__((nonnull));
