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

#ifndef _MORIA_TRAVERSE_INTERNAL_H
#define _MORIA_TRAVERSE_INTERNAL_H

#include "types.h"

#include "valyria/llist.h"

#include "traverse.h"

/*

Vertex and edge entities are in separate traversal state spaces - that is, some vertex and edge may
have the same index and mask. In any particular traversal, either vertices or edges are being
visited (and tracked in the traversal state), never both simultaneously.

*/

typedef struct traversal_state {
  llist lln;        // in graph->states
  struct traversal_state *next;
  size_t size;      // elements in bits
  uint64_t bits[];
} traversal_state;

struct vertex_traversal_state {
  traversal_state st;
};
struct edge_traversal_state {
  traversal_state st;
};

typedef struct entity {
  size_t index;           // (entity id * 3) / 64
  uint64_t mask;          // (entity id * 3) % 64
} entity;

#endif
