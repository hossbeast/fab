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

#ifndef _MORIA_EDGE_H
#define _MORIA_EDGE_H

/*

MODULE
 edge

SUMMARY

REMARKS

*/

#include <stdint.h>

#include "xapi.h"
#include "types.h"

struct graph;
struct vertex;

typedef struct edge
{
  struct vertex * A;      // A depends on B, A -> B
  struct vertex * B;      //  e.g. A is up from B, B is down from A
  uint32_t        attrs;  // properties of the edge

  int visited;            // id of the last traversal to visit this edge

#ifndef EDGE_INTERNALS
# define EDGE_INTERNALS
#endif
  EDGE_INTERNALS;
} edge;

/// edge_disconnect
//
// SUMMARY
//  remove the edge A : B
//
// PARAMETERS
//  g - graph
//  e - edge to remove
//
xapi edge_disconnect(struct graph * restrict g, edge * restrict e)
  __attribute__((nonnull));

edge * edge_between(const struct vertex * restrict A, const struct vertex * restrict B)
  __attribute__((nonnull));

#endif
