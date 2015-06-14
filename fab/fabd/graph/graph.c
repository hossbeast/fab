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

#include "xlinux.h"
#include "xlinux/mempolicy.h"

#include "global.h"
#include "graph.h"
#include "gn.h"
#include "map.h"

#include "memblk.h"

memblk * graph_memblk;
mempolicy * graph_mempolicy;
union graph graph = { { size : sizeof(gn) } };

//
// public
//

void graph_freeze()
{

}

void graph_thaw()
{

}

int graph_setup()
{
  int mpc = 0;

  fatal(memblk_mk, &graph_memblk);
  graph_mempolicy = memblk_getpolicy(graph_memblk);

  fatal(mempolicy_push, graph_mempolicy, &mpc);
  
	fatal(map_create, &graph.by_path, 0);
	fatal(map_create, &graph.by_pathhash, 0);

finally:
  mempolicy_unwind(&mpc);
coda;
}

void graph_teardown()
{
  memblk_free(graph_memblk);
}
