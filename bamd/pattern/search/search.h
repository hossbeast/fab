/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct search_pattern;
struct fsent;
struct hashtable;
struct module;
struct llist;

typedef struct pattern_search_group {
  const char *start; // points into node->name.name
  uint16_t len;
} pattern_search_group;

typedef struct pattern_search_hit {
  struct fsent *node;
  pattern_search_group groups[16];
  uint16_t group_max;
} pattern_search_hit;

/*
 * find nodes matching a pattern
 *
 * pattern        - match pattern
 * module         - module to begin matching at
 * hit_visit      - called when search matches a node
 * dirnode_visit  - called when search visits a directory
 */
void pattern_search(
    /* 1 */ const struct search_pattern * restrict pattern
  , /* 2 */ const struct module * restrict module
  , /* 3 */ void (*hit_visit)(void *ctx, const struct pattern_search_hit *hit)
  , /* 4 */ void *hit_ctx
  , /* 5 */ void (*dirnode_visit)(void *ctx, struct fsent * dirnode)
  , /* 6 */ void *dirnode_ctx
)
  __attribute__((nonnull(1)));

void pattern_search_matches_init(struct hashtable * matches)
  __attribute__((nonnull));
