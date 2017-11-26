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

#ifndef _FF_NODE_RANGE_H
#define _FF_NODE_RANGE_H

#include "ff_node.h"

struct ff_node_char;
struct pattern_match_context;

typedef struct ff_node_range
{
  union {
    ff_node;
    ff_node base;
  };

  struct ff_node_char * start;
  struct ff_node_char * end;
} ff_node_range;

xapi ffn_range_match(struct pattern_match_context * restrict context, const ff_node_range * restrict range)
  __attribute__((nonnull));

#endif
