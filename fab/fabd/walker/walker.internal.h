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
#include "walker.h"

struct filesystem;
struct ftwinfo;
struct node;
struct vertex;

#define restrict __restrict

typedef struct walker_context
{
  struct node * ancestor;
  struct node * root;
  int walk_id;

  xapi (*create)(struct walker_context * restrict ctx, struct node ** restrict n, uint8_t fstype, const struct filesystem * restrict fs, const char * restrict name);
  const struct filesystem * (*fslookup)(struct walker_context * restrict ctx, const char * const restrict path, size_t pathl);
  xapi (*refresh)(struct walker_context * restrict ctx, struct node * restrict n);
  xapi (*watch)(struct walker_context * restrict ctx, struct node * restrict n);
  xapi (*connect)(struct walker_context * restrict ctx, struct node * restrict parent, struct node * restrict n);
  xapi (*disintegrate)(struct walker_context * restrict ctx, struct edge * restrict e);

  void * udata;
} walker_context;

xapi walker_visit(int method, struct ftwinfo * info, void * arg, int * stop)
  __attribute__((nonnull(2)));

#undef restrict
#endif
