/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "search.h"
#include "pattern.h"

struct chain;
struct fsent;
struct module;
struct pattern_section;
struct hashtable;

struct rule;

/// pattern_search_context
//
// SUMMARY
//  dynamic matching context
//
typedef struct pattern_search_context {
  struct search_section_traversal {
    const struct pattern_section *section;
    const llist *head;
  } section_traversal;

  struct search_segments_traversal {
    struct {
      const struct pattern_section *section;
      const union pattern_segment *segment;
    } container;

    // outer segments traversal (lists of qualifiers)
    const llist *segments_head;
    const struct pattern_segments *segments;

    // inner segment traversal (list of eg words)
    const llist *segment_head;
    const union pattern_segment *segment;

    // characters matched by the segments block
    uint16_t start;
    uint16_t offset;  // len is offset - start
    bool unsatisfied; // whether the segments matched

    // MUST be at the end b/c of how RESTORE works!
    struct {
      struct search_segments_traversal *prev;
      struct search_segments_traversal *child;
      struct search_segments_traversal *next;
    } u;

    uint16_t end;
    bool sealed;
  } *traversal;

  // input context
  const struct module * restrict mod;
  void (*dirnode_visit)(void *ctx, struct fsent *dirnode);
  void *dirnode_visit_ctx;

  // state
  const struct fsent *node;
  bool matched;
  pattern_search_group groups[16];
  uint16_t group_max;

  // output context
  struct hashtable *matches;  // struct pattern_search
} pattern_search_context;

typedef struct pattern_search_node {
  struct fsent * node;
  pattern_search_group groups[16];
  uint16_t group_max;
} pattern_search_node;

void pattern_section_search(
    struct pattern_search_context * restrict ctx
  , struct fsent * restrict dirnode
)
  __attribute__((nonnull));

void pattern_segment_search(
    struct pattern_search_context * restrict ctx
  , const union pattern_segment * restrict segment
  , const struct fsent * restrict node
  , uint16_t * restrict name_offset
  , bool * restrict r
)
  __attribute__((nonnull));

void pattern_segments_search(struct pattern_search_context * ctx)
  __attribute__((nonnull));
