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

#ifndef _MORIA_GRAPH_INTERNAL_H
#define _MORIA_GRAPH_INTERNAL_H

#include "types.h"
#include "xapi.h"

#include "graph.h"

struct list;      // valyria/list
struct multimap;  // valyria/multimap

struct vertex;

struct graph
{
  struct list * vertices;
  struct list * edges;
  int traversal_id;

  uint32_t identity;
  struct multimap * mm;

  size_t vsz;
  void (*vertex_value_destroy)(void *);
  xapi (*vertex_value_xdestroy)(void *);
};

xapi graph_vertex_push(graph * const restrict g, struct vertex * const restrict v)
  __attribute__((nonnull));

#endif
