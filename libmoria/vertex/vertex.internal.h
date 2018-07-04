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

struct graph;
struct list;    // valyria/list.h

struct vertex_internals
{
  size_t  up_partition;   // index of the first item in up indexed by vertex
  size_t  down_partition; // index of the first item in down indexed by vertex

  union {
    struct {
      int guard;      // there exists a frame exploring this vertex
      int traveled;   // id of the last traversal to travel this vertex
    };
  };

  char value[];   // opaque user data
};

#define VERTEX_INTERNALS struct vertex_internals
#include "vertex.h"

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
