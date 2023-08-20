/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct pattern;
struct fsent;
struct set;
struct module;
struct llist;

/*
 * find nodes matching a pattern
 *
 * pattern    - match pattern
 * module     - module to begin matching at
 * modules    - all modules
 * [variants] - variants
 * matches    - (returns) matching nodes
 */
void pattern_search(
    /* 1 */ const struct pattern * restrict pattern
  , /* 2 */ const struct module * restrict module
  , /* 3 */ const struct llist * restrict modules
  , /* 4 */ const struct set * restrict variants
  , /* 5 */ struct set * restrict matches
  , /* 6 */ void (*dirnode_visit)(void * ctx, struct fsent * dirnode)
  , /* 7 */ void *dirnode_visit_ctx
)
  __attribute__((nonnull(1, 2, 3)));

void pattern_search_matches_create(struct set ** matches)
  __attribute__((nonnull));
