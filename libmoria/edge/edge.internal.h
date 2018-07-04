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

#ifndef _MORIA_EDGE_INTERNAL_H
#define _MORIA_EDGE_INTERNAL_H

#include <sys/types.h>

struct vertex;

struct edge_internals
{
  size_t up_index;
  size_t down_index;
  size_t edges_index;

  int guard;      // there exists a frame exploring this edge
  int traveled;   // id of the last traversal to travel this edge
};

#define EDGE_INTERNALS struct edge_internals
#include "edge.h"

//  either A or B is specified
struct edge_key_compare_label_context {
  const char * A; // if not null, compare to the edges A vertex
  const char * B; // if not null, compare to the edges B vertex
  uint16_t len;   // length of A or B

  size_t lx;  // (returns) last index
  int lc;     // (returns) results of last comparison
};

//  either A or B is specified
struct edge_key_compare_vertex_context {
  const struct vertex * A; // if not null, compare to the edges A vertex
  const struct vertex * B; // if not null, compare to the edges B vertex

  size_t lx;  // (returns) last index
  int lc;     // (returns) results of last comparison
};

/// edge_key_compare_context
//
// SUMMARY
//
//
int edge_key_compare_label(void * _ctx, const void * _e, size_t idx)
  __attribute__((nonnull));

int edge_key_compare_vertex(void * _ctx, const void * _e, size_t idx)
  __attribute__((nonnull));

#endif
