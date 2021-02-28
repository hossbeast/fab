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

#ifndef _WALKER_INTERNAL_H
#define _WALKER_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "walker.h"

struct ftwinfo;
struct fsent;
struct graph_invalidation_context;

typedef struct walker_context {
  struct fsent * base;        // base of the tree
  struct fsent * base_parent; // base attached here if/when created
  int walk_id;
  struct graph_invalidation_context * invalidation;
} walker_context;

xapi walker_visit(int method, struct ftwinfo * info, void * arg, int * stop)
  __attribute__((nonnull(2)));

#endif
