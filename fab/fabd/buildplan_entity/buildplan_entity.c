/* Copyright (c) 2012-2018 Todd Freed <todd.freed@gmail.com>

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

#include "moria/edge.h"
#include "narrator.h"

#include "buildplan_entity.internal.h"
#include "node.h"

//
// public
//

xapi bpe_say_sources(buildplan_entity * restrict bpe, narrator * restrict N)
{
  enter;

  node *n;
  node_edge *ne;
  edge *e;
  int x;

  if(bpe->typemark == BPE_NODE)
  {
    n = containerof(bpe, typeof(*n), self_bpe);
    fatal(node_path_say, n, N);
  }
  else if(bpe->typemark == BPE_NODE_EDGE)
  {
    ne = containerof(bpe, typeof(*ne), bpe);
    e = edge_containerof(ne);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      if(ne->dir == EDGE_TGT_SRC)
        n = vertex_value(e->B);
      else
        n = vertex_value(e->A);
      fatal(node_path_say, n, N);
    }
    else if(ne->dir == EDGE_TGT_SRC)
    {
      for(x = 0; x < e->Blen; x++)
      {
        n = vertex_value(e->Blist[x].v);
        if(x)
          fatal(narrator_xsays, N, ",");
        fatal(node_path_say, n, N);
      }
    }
    else if(ne->dir == EDGE_SRC_TGT)
    {
      for(x = 0; x < e->Alen; x++)
      {
        n = vertex_value(e->Alist[x].v);
        if(x)
          fatal(narrator_xsays, N, ",");
        fatal(node_path_say, n, N);
      }
    }
    else
    {
      RUNTIME_ABORT();
    }
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}

xapi bpe_say_targets(buildplan_entity * restrict bpe, narrator * restrict N)
{
  enter;

  node *n;
  node_edge *ne;
  edge *e;
  int x;

  if(bpe->typemark == BPE_NODE)
  {
    n = containerof(bpe, typeof(*n), self_bpe);
    fatal(node_path_say, n, N);
  }
  else if(bpe->typemark == BPE_NODE_EDGE)
  {
    ne = containerof(bpe, typeof(*ne), bpe);
    e = edge_containerof(ne);
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      if(ne->dir == EDGE_TGT_SRC)
        n = vertex_value(e->A);
      else
        n = vertex_value(e->B);
      fatal(node_path_say, n, N);
    }
    else if(ne->dir == EDGE_TGT_SRC)
    {
      for(x = 0; x < e->Alen; x++)
      {
        n = vertex_value(e->Alist[x].v);
        if(x)
          fatal(narrator_xsays, N, ", ");
        fatal(node_path_say, n, N);
      }
    }
    else if(ne->dir == EDGE_SRC_TGT)
    {
      for(x = 0; x < e->Blen; x++)
      {
        n = vertex_value(e->Blist[x].v);
        if(x)
          fatal(narrator_xsays, N, ", ");
        fatal(node_path_say, n, N);
      }
    }
    else
    {
      RUNTIME_ABORT();
    }
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}
