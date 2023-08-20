/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
void pattern_segment_generate(pattern_generate_context * restrict context)
  __attribute__((nonnull));
