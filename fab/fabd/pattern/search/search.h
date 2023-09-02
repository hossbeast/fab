/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct pattern;
struct fsent;
struct hashtable;
struct module;
struct llist;

/*
 * find nodes matching a pattern
 *
 * pattern    - match pattern
 * module     - module to begin matching at
 * matches    - (returns) matching nodes
 */
void pattern_search(
    /* 1 */ const struct pattern * restrict pattern
  , /* 2 */ const struct module * restrict module
  , /* 3 */ struct fsent * restrict base
  , /* 4 */ struct hashtable * restrict matches
  , /* 5 */ void (*dirnode_visit)(void * ctx, struct fsent * dirnode)
  , /* 6 */ void *dirnode_visit_ctx
)
  __attribute__((nonnull(1, 2)));

void pattern_search_matches_init(struct hashtable * matches)
  __attribute__((nonnull));
