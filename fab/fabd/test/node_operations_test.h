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

#ifndef _FABD_NODE_OPERATIONS_TEST_H
#define _FABD_NODE_OPERATIONS_TEST_H

struct narrator;
struct fsent;
struct llist;

/// node_operations_dispatch
//
// function pointers for moria/operations which operate on g_graph
//
extern struct moria_operations_dispatch * node_operations_test_dispatch;

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

#endif
