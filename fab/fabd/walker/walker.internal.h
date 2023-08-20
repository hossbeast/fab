/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "walker.h"

struct ftwinfo;
struct fsent;
struct graph_invalidation_context;
struct llist;

extern struct llist * walker_exclude_list;

typedef struct walker_context {
  struct fsent * base;        // base of the tree
  struct fsent * base_parent; // base attached here if/when created
  int walk_id;
  struct graph_invalidation_context * invalidation;
} walker_context;

void walker_visit(int method, struct ftwinfo * info, void * arg, int * stop)
  __attribute__((nonnull(2)));
