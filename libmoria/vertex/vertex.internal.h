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

struct list;    // valyria/list.h

typedef struct vertex
{
  VERTEX_VALUE_TYPE * value;

  struct list * up;    // edges where this == B, i.e. { up } -> this
  struct list * down;  // edges where this == A, i.e. this -> { down }

  // buildplan create tracking
  int                 height;   // distance of longest route to a leaf node
  int                 stage;    // assigned stage - NEARLY always equal to height

  // traversal tracking
  int                 guard;
  int                 visited;
  int                 traversed;
} vertex;

/// vertex_create
//
// SUMMARY
//  initialize a vertex
//
xapi vertex_create(vertex ** const restrict v)
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
