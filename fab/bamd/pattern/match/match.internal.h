/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "match.h"

struct pattern_search_context;
struct fsent;
struct pattern_match_context;
struct pattern_segments;
struct pattern_section;

void pattern_section_match(
    struct pattern_search_context * restrict ctx
  , const struct pattern_section * restrict prev
  , const struct fsent * restrict dirnode
)
  __attribute__((nonnull(1, 3)));

void pattern_segments_match(struct pattern_search_context * ctx)
  __attribute__((nonnull));

typedef struct pattern_match_context {
  struct match_section_traversal {
    const struct pattern_section * section;
    const struct pattern_section * head;
  } section_traversal;

  struct match_segments_traversal {
    struct {
      const struct pattern_section * section;
      const union pattern_segment * segment;
    } container;

    // segments traversal
    const struct pattern_segments * segments;
    const struct pattern_segments * segments_head;

    // within the current segments
    const union pattern_segment * segment;
    const union pattern_segment * segment_head;

    // characters matched by the segments block
    uint16_t start;
    uint16_t offset;  // len is offset - start

    bool qualifiers;  // whether any type qualifiers were checked
    bool satisfied;   // result of type qualifier checks

    // MUST be at the end b/c of how RESTORE works!
    struct {
      struct match_segments_traversal *prev;
      struct match_segments_traversal *child;
      struct match_segments_traversal *next;
      bool match;
    } u;
  } *traversal;

  // state
  const struct fsent * dirnode;
  const char * label;
  uint16_t label_len;

  // output context
  bool matched;
} pattern_match_context;
