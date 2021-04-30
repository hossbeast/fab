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

#ifndef FABD_PATTERN_GENERATE_INTERNAL_H
#define FABD_PATTERN_GENERATE_INTERNAL_H

#include "valyria/llist.h"

#include "generate.h"
#include "types.h"

struct chain;
struct set;
struct narrator;
struct narrator_fixed;
struct pattern_search_node;
struct pattern_section;
struct variant;
struct graph_invalidation_context;
struct moria_vertex;

/// pattern_generate_context
//
// SUMMARY
//  dynamic generate context
//
// MEMBERS
//  node             - new nodes are created as children of this node
//  segment_narrator - narrator where the current section is accumulated
//  segment_base_pos - position in the narrator where the current section begins
//  variant_pos      - offset in the name of the context node where the current variant begins
//  variant_len      - length of the current variant
//
typedef struct pattern_generate_context
{
  // section traversal
  struct section_traversal {
    const struct pattern_section * section;
    const struct pattern_section * head;
    const struct chain * cursor;
    int16_t variant_index;
    const struct variant * variant_replacement;
  } section_traversal;

  // segment traversal
  struct segment_traversal {
    llist lln;
    const union pattern_segment * segment;
    const union pattern_segment * head;
    const struct chain * cursor;
  } *segment_traversal;

  llist segment_traversal_stack;

  // state
  struct fsent * node;
  union {
    struct narrator * restrict section_narrator;
    struct narrator_fixed * restrict section_narrator_fixed;
  };
  off_t section_narrator_pos;
  struct graph_invalidation_context * invalidation;

  // input context
  struct fsent * base;
  struct module * mod;
  struct moria_vertex * scope;
  const struct pattern_search_node * match;
  const struct set * restrict variants;

  // output context
  struct set * nodes;
} pattern_generate_context;

/// pattern_segment_generate
//
// SUMMARY
//  generate nodes from a segment of a pattern
//
// PARAMETERS - as for pattern_generate
//  context - dynamic context
//  walk    - by-directories-segment ffn walk
//
xapi pattern_segment_generate(pattern_generate_context * restrict context)
  __attribute__((nonnull));

#endif
