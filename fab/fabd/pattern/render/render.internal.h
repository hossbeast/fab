/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#ifndef PATTERN_RENDER_INTERNAL_H
#define PATTERN_RENDER_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"

#include "render.h"

struct chain;
struct narrator;
struct narrator_growing;
struct pattern_section;

/// pattern_render_context
//
// SUMMARY
//  dynamic render context
//
// MEMBERS
//  node         - new nodes are created as children of this node
//  narrator     - narrator where the current section is accumulated
//  narrator_pos - position in the narrator where the current section begins
//
typedef struct pattern_render_context
{
  // section traversal
  struct render_section_traversal {
    const struct pattern_section * section;
    const struct pattern_section * head;
    const struct chain * cursor;
  } section_traversal;

  // segment traversal
  struct render_segment_traversal {
    llist lln;
    const union pattern_segment * segment;
    const union pattern_segment * head;
    const struct chain * cursor;
  } *segment_traversal;

  llist segment_traversal_stack;

  // state
  union {
    struct narrator * narrator;
    struct narrator_growing * narrator_growing;
  };
  off_t pos;

  // results
  uint16_t size;
} pattern_render_context;

/// pattern_segment_render
//
// SUMMARY
//  render nodes from a segment of a pattern
//
// PARAMETERS - as for pattern_render
//  context - dynamic context
//  walk    - by-directories-segment ffn walk
//
xapi pattern_segment_render(pattern_render_context * restrict context)
  __attribute__((nonnull));

#endif
