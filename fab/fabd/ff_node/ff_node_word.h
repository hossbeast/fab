/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef FF_NODE_WORD_H
#define FF_NODE_WORD_H

#include "types.h"
#include "xapi.h"

#include "ff_node.h"

struct artifact;
struct ffn_bydir_walk;
struct list;
struct pattern_match_context;
struct pattern_generate_context;

typedef struct ff_node_word
{
  ff_node;

  char * text;
  size_t len;
} ff_node_word;

/// ffn_word_match
//
// see pattern_part_match
//
xapi ffn_word_match(struct pattern_match_context * restrict context, struct ffn_bydir_walk * restrict walk)
  __attribute__((nonnull));

/// ffn_word_generate
//
// see pattern_segment_generate
//
xapi ffn_word_generate(
    /* 1 */ struct pattern_generate_context * restrict context
  , /* 2 */ struct ffn_bydir_walk * restrict walk
  , /* 3 */ const struct artifact * restrict context_af
  , /* 4 */ const char * restrict stem
  , /* 5 */ uint16_t stem_len
  , /* 6 */ struct list * restrict results
  , /* 7 */ bool generating_artifact
)
  __attribute__((nonnull(1, 2, 6)));

#endif
