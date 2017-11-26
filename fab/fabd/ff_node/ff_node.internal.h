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

#ifndef FF_NODE_INTERNAL_H
#define FF_NODE_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "ff_node.h"

struct narrator;
union ff_node_pattern_part;

xapi ffn_say_normal_list(const ff_node * restrict n, struct narrator * restrict N, const char * restrict sep)
  __attribute__((nonnull(2)));

xapi ffn_say_tree_level(const ff_node * restrict n, int level, struct narrator * restrict N)
  __attribute__((nonnull(3)));

#endif
