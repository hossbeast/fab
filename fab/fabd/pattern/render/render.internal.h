/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
void pattern_segment_render(pattern_render_context * restrict context)
  __attribute__((nonnull));
