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

#include <string.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "valyria/list.h"

#include "internal.h"
#include "edge.internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"

#include "strutil.h"

#define restrict __restrict

//
// api
//

API xapi edge_disconnect(graph * const restrict g, edge * const restrict e)
{
  enter;

  int x;

  fatal(list_delete, e->A->down, e->down_index);
  for(x = e->down_index; x < e->A->down->l; x++)
    ((edge*)list_get(e->A->down, x))->down_index--;

  fatal(list_delete, e->B->up, e->up_index);
  for(x = e->up_index; x < e->B->up->l; x++)
    ((edge*)list_get(e->B->up, x))->up_index--;

  size_t edges_index = e->edges_index;
  fatal(list_delete, g->edges, e->edges_index);
  for(x = edges_index; x < g->edges->l; x++)
    ((edge*)list_get(g->edges, x))->edges_index--;

  finally : coda;
}
