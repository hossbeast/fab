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

#ifndef _FF_NODE_PATTERNS_H
#define _FF_NODE_PATTERNS_H

#include "ff_node.h"

struct ff_node_pattern;

typedef struct ff_node_patterns
{
  union {
    ff_node;
    ff_node base;
  };

  struct ff_node_pattern * chain;
} ff_node_patterns;

// sequence of path fragments rendered from a pattern
typedef struct __attribute__((packed)) rendered_patterns {
  uint16_t num;
  struct __attribute__((packed)) rendered_item {
    uint16_t len;
    char name[];
  } items[];
} rendered_patterns;

static inline struct rendered_item * rendered_item_next(struct rendered_item * item)
{
  return (typeof(item))(((char*)(item + 1)) + item->len + 1);
}

/// ffn_patterns_render
//
// SUMMARY
//  resolve metacharacters in a list of patterns to produce a set of path fragments
//
// PARAMETERS
//  pats  - list of patterns
//  block - (returns) rendered path fragments
//
xapi ffn_patterns_render(const ff_node_patterns * restrict pats, rendered_patterns ** restrict block)
  __attribute__((nonnull));

#endif
