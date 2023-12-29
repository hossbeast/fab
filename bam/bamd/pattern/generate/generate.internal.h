/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

#include "generate.h"
#include "types.h"

struct chain;
struct narrator;
struct narrator_fixed;
struct pattern_search_node;
struct pattern_section;
struct graph_invalidation_context;
struct moria_vertex;
struct entlist;

/// pattern_generate_context
//
// SUMMARY
//  dynamic generate context
//
// MEMBERS
//  node             - new nodes are created as children of this node
//  segment_narrator - narrator where the current section is accumulated
//  segment_base_pos - position in the narrator where the current section begins
//
typedef struct pattern_generate_context {
  // section traversal
  struct section_traversal {
    const llist *head;
    const struct pattern_section *section;
  } section_traversal;

  // segment traversal
  struct segment_traversal {
    llist lln;
    const llist *head;
    const union pattern_segment *segment;
    bool end;
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
  uint32_t flags;

  // output context
  struct entlist *hits;
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
