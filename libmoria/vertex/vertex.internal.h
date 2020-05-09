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

#include "vertex.h"
#include "macros.h"

#include "traverse.internal.h"

struct graph;
struct list;    // valyria/list.h

typedef struct vertex_t {
  union {
    vertex vx;
    struct {
      fieldof(vertex, label);
      fieldof(vertex, label_len);
      fieldof(vertex, attrs);
      fieldof(vertex, up);
      fieldof(vertex, up_identity);
      fieldof(vertex, down);
      fieldof(vertex, ref);
      fieldof(vertex, lln);
      fieldof(vertex, graph_lln);
    };
  };

  entity ent;     // traversable entity
  char value[];   // opaque user data
} vertex_t;

STATIC_ASSERT(offsetof(vertex_t, value) % 8 == 0);

ALIGNEDOF(vertex, vertex_t, label);
ALIGNEDOF(vertex, vertex_t, label_len);
ALIGNEDOF(vertex, vertex_t, attrs);
ALIGNEDOF(vertex, vertex_t, up);
ALIGNEDOF(vertex, vertex_t, down);
ALIGNEDOF(vertex, vertex_t, ref);
ALIGNEDOF(vertex, vertex_t, lln);
ALIGNEDOF(vertex, vertex_t, graph_lln);

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

xapi vertex_delete(vertex_t * restrict v, struct graph * restrict g)
  __attribute__((nonnull));

#endif
