/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct module;
struct pattern_search_hit;
struct vertices;
struct graph_invalidation_context;
struct fsent;
struct generate_pattern;
struct attachment_pattern;
struct include_pattern;
struct bacon_value;
struct bacon_parser;
struct moria_vertex;

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
    /*  1 */ const struct generate_pattern * restrict pat
  , /*  2 */ struct module * restrict module
  , /*  3 */ struct bacon_value * restrict vars
  , /*  4 */ struct bacon_parser * restrict parser
  , /*  5 */ struct fsent * restrict base
  , /*  6 */ struct graph_invalidation_context * invalidation
  , /*  7 */ const struct pattern_search_hit * restrict match
  , /*  8 */ uint32_t flags
  , /*  9 */ void (*hit_visit)(void *ctx, const struct moria_vertex *hit)
  , /* 10 */ void *hit_ctx
)
  __attribute__((nonnull(1, 6, 9)));

void pattern_attachment_generate(
    /*  1 */ const struct attachment_pattern * restrict pat
  , /*  2 */ struct module * restrict module
  , /*  3 */ struct bacon_value * restrict vars
  , /*  4 */ struct bacon_parser * restrict parser
  , /*  5 */ struct fsent * restrict base
  , /*  6 */ struct graph_invalidation_context * invalidation
  , /*  7 */ const struct pattern_search_hit * restrict match
  , /*  8 */ uint32_t flags
  , /*  9 */ void (*hit_visit)(void *ctx, const struct moria_vertex *hit)
  , /* 10 */ void *hit_ctx
)
  __attribute__((nonnull(1, 6, 9)));

void pattern_include_generate(
    /*  1 */ const struct include_pattern * restrict pat
  , /*  2 */ struct module * restrict module
  , /*  3 */ struct bacon_value * restrict vars
  , /*  4 */ struct bacon_parser * restrict parser
  , /*  5 */ struct fsent * restrict base
  , /*  6 */ struct graph_invalidation_context * invalidation
  , /*  7 */ const struct pattern_search_hit * restrict match
  , /*  8 */ uint32_t flags
  , /*  9 */ void (*hit_visit)(void *ctx, const struct moria_vertex *hit)
  , /* 10 */ void *hit_ctx
)
  __attribute__((nonnull(1, 6, 9)));
