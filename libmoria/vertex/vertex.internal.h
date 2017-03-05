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

#ifndef _MORIA_VERTEX_INTERNAL_H
#define _MORIA_VERTEX_INTERNAL_H

struct list;    // valyria/list.h

struct vertex_internals
{
  int guard;      // there exists a frame exploring this vertex
  int traveled;   // id of the last traversal to travel this vertex

  char value[];   // opaque user data
};

#define VERTEX_INTERNALS struct vertex_internals
#include "vertex.h"

struct vertex_cmp_context {
  size_t lx;
  int lc;
  const char * A;
  const char * B;
  size_t len;
  size_t off;
};

int vertex_compare(void * _ctx, const void * _e, size_t idx)
  __attribute__((nonnull));

/// vertex_create
//
// SUMMARY
//  allocate a vertex
//
xapi vertex_create(vertex ** const restrict v, size_t vsz, uint32_t attrs)
  __attribute__((nonnull));

xapi vertex_createw(vertex ** const restrict v, size_t vsz, uint32_t attrs, const char * const restrict label, size_t label_len)
  __attribute__((nonnull));

/// vertex_free
//
// SUMMARY
//  free a vertex
//
xapi vertex_xfree(vertex * restrict v);

/// vertex_ifree
//
// SUMMARY
//  free a vertex, zero its reference
//
xapi vertex_ixfree(vertex ** const restrict v)
  __attribute__((nonnull));

#endif
