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

#include <sys/types.h>
#include "xapi.h"

#include "graph.h"

struct list;

struct graph
{
  struct list * vertices;
  struct list * edges;
  int traversal_id;

  size_t vsz;
  void (*vertex_value_destroy)(void *);
  xapi (*vertex_value_xdestroy)(void *);
};

#endif
