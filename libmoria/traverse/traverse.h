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

#ifndef _MORIA_TRAVERSE_H
#define _MORIA_TRAVERSE_H

/*

MODULE
 moria/traverse

*/

#include <stdint.h>

/* traversal options and modifiers */
#define MORIA_ATTRS_TABLE                                                                                \
  MORIA_ATTR(MORIA_TRAVERSE_UP   , 0x00000001) /* traverse from a node to its consumers (upward) */      \
  MORIA_ATTR(MORIA_TRAVERSE_DOWN , 0x00000002) /* traverse from a node to its dependencies (downward) */ \
  MORIA_ATTR(MORIA_TRAVERSE_PRE  , 0x00000004) /* breadth-first traversal */                             \
  MORIA_ATTR(MORIA_TRAVERSE_POST , 0x00000008) /* depth-first traversal */

enum {
#define MORIA_ATTR(name, number) name = number,
MORIA_ATTRS_TABLE
#undef MORIA_ATTR
};

/// traversal_criteria
//
// SUMMARY
//  Parameters for selecting vertices and edges to consider during a traversal. An entity (a vertex, or an edge) is said
//  to be visited when the visitor callback is invoked on it. An entity is said to be traveled when entities on the far
//  side of it, from the perspective of the traversal, are considered as candidates for visitation.
//
typedef struct traversal_criteria {
  uint32_t vertex_travel;   // bitmask
  uint32_t vertex_visit;
  uint32_t edge_travel;
  uint32_t edge_visit;
} traversal_criteria;

#endif
