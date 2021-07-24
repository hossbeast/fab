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

#ifndef FABD_PATTERN_MATCH_INTERNAL_H
#define FABD_PATTERN_MATCH_INTERNAL_H

#include "types.h"

#include "match.h"

struct fsent;
struct pattern_match_context;
struct pattern_segments;

void pattern_section_match(
    struct pattern_match_context * restrict ctx
  , const struct fsent * restrict dirnode
)
  __attribute__((nonnull));

void pattern_segments_match(struct pattern_match_context * ctx)
  __attribute__((nonnull));

typedef struct pattern_match_context {
  struct match_section_traversal {
    const struct pattern_section * section;
    const struct pattern_section * head;
    const struct chain * cursor;
  } section_traversal;

  struct match_segments_traversal {
    struct {
      const struct pattern_section * section;
      const union pattern_segment * segment;
    } container;

    // segments traversal
    const struct pattern_segments * segments;
    const struct pattern_segments * segments_head;
    const struct chain * segments_cursor;

    // within the current segments
    const union pattern_segment * segment;
    const union pattern_segment * segment_head;
    const struct chain * segment_cursor;

    // characters matched by the segments block
    uint16_t start;
    uint16_t offset;  // len is offset - start

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

#endif
