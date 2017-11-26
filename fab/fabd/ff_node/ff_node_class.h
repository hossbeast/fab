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

#ifndef _FF_NODE_CLASS_H
#define _FF_NODE_CLASS_H

#include "types.h"

#include "ff_node.h"

#include "ff_node_char.h"
#include "ff_node_range.h"

struct artifact;
struct node;
struct list;
union ff_node_pattern_part;
struct pattern_generate_context;
struct ffn_bydir_walk;
struct pattern_match_context;

typedef union ff_node_class_part
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_char character;
  ff_node_range range;
} ff_node_class_part;

typedef struct ff_node_class
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_class_part * chain;
} ff_node_class;

xapi ffn_class_match(struct pattern_match_context * restrict context, struct ffn_bydir_walk * restrict walk)
  __attribute__((nonnull));

xapi ffn_class_generate(
    /* 1 */ struct pattern_generate_context * restrict context
  , /* 2 */ struct ffn_bydir_walk * restrict walk
  , /* 3 */ const struct artifact * restrict context_af
  , /* 3 */ const char * restrict stem
  , /* 4 */ uint16_t stem_len
  , /* 5 */ struct list * restrict results
  , /* 6 */ uint8_t opts
)
  __attribute__((nonnull(1, 2, 6)));

#endif
