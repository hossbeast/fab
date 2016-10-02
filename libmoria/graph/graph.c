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

#include <sys/types.h>
#include <stddef.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "errtab/MORIA.errtab.h"

typedef struct edge
{
  struct vertex * A;      // A depends on B, A -> B
  struct vertex * B;
  uint64_t        attrs;  // properties of the edge
} edge;

struct graph
{
  list * vertices;
  list * edges;
  xapi (* say)(vertex * const restrict v, struct narrator * const restrict N);
  int traversal_ids;
};


#define DIRECTION_OPT   UINT32_C(0x00000003)
#define METHOD_OPT      UINT32_C(0x0000000c)

#define GRAPH_TRAVERSE_DEF(a, b, x, m) (x & m) == b ? #a :
#define DIRECTION_VALUE(x)  GRAPH_TRAVERSE_TABLE(x, DIRECTION_OPT) "NONE"
#define METHOD_VALUE(x)     GRAPH_TRAVERSE_TABLE(x, METHOD_OPT) "NONE"

#define restrict __restrict

//
// static
//

static int traverse(
    vertex * const restrict v
  , xapi (* const visit)(vertex * const restrict, void *)
  , uint32_t skip
  , uint32_t finish
  , uint32_t stop
  , void * arg
  , int id
  , uint32_t direction
  , uint32_t method
  , int vtraverse
  , int vvisit
  , vertex * (* const restrict stack)[32]
  , size_t * restrict stackz
)
{
  enter;

#if 0
printf("\n");
printf("TRAVERSE %p\n", v);
printf("traverse %d\n", vtraverse);
printf("visit %d\n", vvisit);
#endif

  if(v->guard)  // cycle detected
    fail(MORIA_CYCLE);

  v->guard = 1; // descend

  if(method == GRAPH_TRAVERSE_BREADTH && vvisit && v->visited != id)
  {
    fatal(visit, v, arg);
    v->visited = id;
  }

  if(vtraverse && v->traversed != id)
  {
    int x = -1;
    while(1)
    {
      x++;
      edge * e = 0;
      vertex * other = 0;
      if(direction == GRAPH_TRAVERSE_UP)
      {
        if(x == list_size(v->up))
          break;
        e = list_get(v->up, x);
        other = e->A;
      }

      else if(direction == GRAPH_TRAVERSE_DOWN)
      {
        if(x == list_size(v->down))
          break;
        e = list_get(v->down, x);
        other = e->B;
      }

      int ntraverse = !(e->attrs & (finish | stop));
      int nvisit = !(e->attrs & (skip | stop));

      fatal(traverse
        , other
        , visit
        , skip
        , finish
        , stop
        , arg
        , id
        , direction
        , method
        , ntraverse
        , nvisit
        , stack
        , stackz
      );
    }

    v->traversed = id;
  }

  if(method == GRAPH_TRAVERSE_DEPTH && vvisit && v->visited != id)
  {
    fatal(visit, v, arg);
    v->visited = id;
  }

  v->guard = 0; // ascend

finally:
  if(XAPI_THROWING(MORIA_CYCLE))
  {
    if((*stackz) < sizeof((*stack)) / sizeof((*stack)[0]))
      (*stack)[(*stackz)++] = v;
  }
coda;
}


//
// api
//

API xapi graph_create(graph ** const restrict g, xapi (* say)(struct vertex * const restrict v, struct narrator * const restrict N))
{
  enter;

  fatal(xmalloc, g, sizeof(**g));
  fatal(list_createx, &(*g)->vertices, 0, (void*)vertex_xfree, 0);
  fatal(list_createx, &(*g)->edges, (void*)wfree, 0, 0);

  (*g)->say = say;

  finally : coda;
}

API xapi graph_xfree(graph * const restrict g)
{
  enter;

  if(g)
  {
    fatal(list_xfree, g->vertices);
    fatal(list_xfree, g->edges);
  }

  wfree(g);

  finally : coda;
}

API xapi graph_ixfree(graph ** const restrict g)
{
  enter;

  fatal(graph_xfree, *g);
  *g = 0;

  finally : coda;
}

xapi graph_vertex_create(graph * const restrict g, struct vertex ** const restrict v)
{
  enter;

  vertex * lv = 0;
  fatal(vertex_create, &lv);

  fatal(list_push, g->vertices, (void*)lv);
  *v = lv;
  lv = 0;

finally:
  fatal(vertex_xfree, lv);
coda;
}

API xapi graph_relate(graph * const restrict g, vertex * const restrict A, vertex * const restrict B, uint32_t attrs)
{
  enter;

  edge * e = 0;

  // binary search for the edge
  int compar(void * key, const void * item, size_t idx)
  {
    ptrdiff_t diff = key - (void*)((edge *)item)->B;
    return diff > 0 ? 1 : diff < 1 ? -1 : 0;
  };
  if(list_search(A->down, B, compar) == 0)
  {
    // no such edge
    fatal(xmalloc, &e, sizeof(*e));
    e->A = A;
    e->B = B;
    e->attrs = attrs;

    fatal(list_push, A->down, e);
    fatal(list_push, B->up, e);

    fatal(list_push, g->edges, e);
    e = 0;
  }

finally:
  wfree(e);
coda;
}

API xapi graph_traverse(
    graph * const restrict g
  , vertex * const restrict v
  , xapi (* const visit)(vertex * const restrict, void *)
  , uint32_t skip
  , uint32_t finish
  , uint32_t stop
  , uint32_t attrs
  , void * arg
)
{
  enter;

  narrator * N = 0;
  vertex * stack[32] = {};
  size_t stackz = 0;

  int id = ++g->traversal_ids;

  fatal(traverse
    , v
    , visit
    , skip
    , finish
    , stop
    , arg
    , id
    , attrs & DIRECTION_OPT
    , attrs & METHOD_OPT
    , 1         // traverse
    , 1         // visit
    , &stack
    , &stackz
  ); 

finally:
  if(XAPI_THROWING(MORIA_CYCLE))
  {
    fatal(narrator_fixed_create, &N, 4096);

    int x;
    for(x = stackz - 1; x >= 0; x--)
    {
      if(narrator_fixed_size(N))
        says(" -> ");
      fatal(g->say, stack[x], N);
    }

    xapi_infos("path", narrator_fixed_buffer(N));
  }

  fatal(narrator_xfree, N);
coda;
}
