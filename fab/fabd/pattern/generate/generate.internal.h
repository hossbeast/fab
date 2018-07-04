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

#include "generate.h"
#include "types.h"

struct ff_node_pattern;
struct ffn_bydir_context;
struct ffn_bydir_walk;
struct list;
struct narrator;
union ff_node_pattern_part;

/// pattern_generate_context
//
// SUMMARY
//  dynamic generate context
//
// MEMBERS
//  node             - new nodes are created as children of this node
//  segment_narrator - narrator where the current slash-delimited segment is accumulated
//  segment_base_pos - position in the narrator where the current slash-delimited segment begins
//  variant_pos      - offset in the name of the context node where the current variant begins
//  variant_len      - length of the current variant
//
typedef struct pattern_generate_context
{
  struct node * restrict node;
  struct narrator * restrict segment_narrator;
  off_t segment_base_pos;
  off_t variant_pos;
  int variant_len;
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
xapi pattern_segment_generate(
    /* 1 */ pattern_generate_context * restrict context
  , /* 2 */ struct ffn_bydir_walk * restrict walk
  , /* 3 */ const struct artifact * restrict af 
  , /* 4 */ const char * restrict stem
  , /* 5 */ uint16_t stem_len
  , /* 6 */ struct list * restrict results
  , /* 7 */ bool generating_artifact
)
  __attribute__((nonnull(1, 2, 6)));

#endif
