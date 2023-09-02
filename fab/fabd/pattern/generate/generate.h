/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct module;
struct pattern;
struct pattern_search_node;
struct entlist;
struct graph_invalidation_context;
struct fsent;

#define PATTERN_GENERATE_NOFOLLOW 1

/*
 * create nodes as specified by a pattern
 *
 * pattern  - generate pattern
 * base     - node to which new nodes are attached (directory node for the module)
 * scope    - if specified, generation begins at this dir node
 * results  - newly created nodes are appended to this list
 */
void pattern_generate(
    /* 1 */ const struct pattern * restrict pat
  , /* 2 */ struct module * restrict module
  , /* 3 */ struct fsent * restrict base
  , /* 4 */ struct graph_invalidation_context * invalidation
  , /* 5 */ const struct pattern_search_node * restrict match
  , /* 6 */ struct entlist * restrict results
  , /* 7 */ uint32_t flags
)
  __attribute__((nonnull(1, 4, 6)));
