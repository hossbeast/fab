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
#include <string.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "edge.internal.h"
#include "errtab/MORIA.errtab.h"

#define DIRECTION_OPT   UINT32_C(0x00000003)
#define METHOD_OPT      UINT32_C(0x0000000c)

#define GRAPH_TRAVERSE_DEF(a, b, x, m) (x & m) == b ? #a :
#define DIRECTION_VALUE(x)  GRAPH_TRAVERSE_TABLE(x, DIRECTION_OPT) "NONE"
#define METHOD_VALUE(x)     GRAPH_TRAVERSE_TABLE(x, METHOD_OPT) "NONE"

#define restrict __restrict

//
// static
//

static xapi __attribute__((nonnull(1, 13, 14))) explore_vertex(
    /*  1 */ vertex * const restrict v
  , /*  2 */ xapi (* const visitor)(vertex * const restrict, int, void *)
  , /*  3 */ uint32_t vertex_travel
  , /*  4 */ uint32_t vertex_visit
  , /*  5 */ uint32_t edge_travel
  , /*  6 */ uint32_t edge_visit
  , /*  7 */ uint32_t attrs
  , /*  8 */ void * ctx
  , /*  9 */ int traversal_id
  , /* 10 */ int distance
  , /* 11 */ int travel
  , /* 12 */ int visit
  , /* 13 */ vertex * (* const restrict stack)[32]
  , /* 14 */ size_t * const restrict stackz
)
{
  enter;

  edge * next_edge = 0;
  vertex * next_vertex = 0;
  int next_travel;
  int next_visit;

  if(v->guard)  // cycle detected
    fail(MORIA_CYCLE);

  v->guard = 1; // descend

  if((attrs & METHOD_OPT) == GRAPH_TRAVERSE_PRE && visit && v->visited != traversal_id)
  {
    v->visited = traversal_id;
    if(visitor)
      fatal(visitor, v, distance, ctx);
  }

  if(travel && v->traveled != traversal_id)
  {
    v->traveled = traversal_id;
    int x = -1;
    while(1)
    {
      x++;
      if((attrs & DIRECTION_OPT) == GRAPH_TRAVERSE_UP)
      {
        if(x == list_size(v->up))
          break;
        next_edge = list_get(v->up, x);
        next_vertex = next_edge->A;
      }
      else if((attrs & DIRECTION_OPT) == GRAPH_TRAVERSE_DOWN)
      {
        if(x == list_size(v->down))
          break;
        next_edge = list_get(v->down, x);
        next_vertex = next_edge->B;
      }

      next_travel = 1;
      if(vertex_travel)
        next_travel &= !!(next_vertex->attrs & vertex_travel);
      if(edge_travel)
        next_travel &= !!(next_edge->attrs & edge_travel);

      next_visit = 1;
      if(vertex_visit)
        next_visit &= !!(next_vertex->attrs & vertex_visit);
      if(edge_visit)
        next_visit &= !!(next_edge->attrs & edge_visit);

      if(next_travel || next_visit)
      {
        fatal(explore_vertex
          , next_vertex
          , visitor
          , vertex_travel
          , vertex_visit
          , edge_travel
          , edge_visit
          , attrs
          , ctx
          , traversal_id
          , distance + 1
          , next_travel
          , next_visit
          , stack
          , stackz
        );
      }
    }
  }

  if((attrs & METHOD_OPT) == GRAPH_TRAVERSE_POST && visit && v->visited != traversal_id)
  {
    v->visited = traversal_id;
    if(visitor)
      fatal(visitor, v, distance, ctx);
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

static xapi __attribute__((nonnull(1, 13, 14))) explore_edge(
    /*  1 */ edge * const restrict e
  , /*  2 */ xapi (* const visitor)(edge * const restrict, int, void *)
  , /*  3 */ uint32_t vertex_travel
  , /*  4 */ uint32_t vertex_visit
  , /*  5 */ uint32_t edge_travel
  , /*  6 */ uint32_t edge_visit
  , /*  7 */ uint32_t attrs
  , /*  8 */ void * ctx
  , /*  9 */ int traversal_id
  , /* 10 */ int distance
  , /* 11 */ int travel
  , /* 12 */ int visit
  , /* 13 */ edge * (* const restrict stack)[32]
  , /* 14 */ size_t * const restrict stackz
)
{
  enter;

  edge * next_edge = 0;
  vertex * next_vertex = 0;
  int next_travel;
  int next_visit;

  if(e->guard)  // cycle detected
    fail(MORIA_CYCLE);

  e->guard = 1; // descend

  if((attrs & METHOD_OPT) == GRAPH_TRAVERSE_PRE && visit && e->visited != traversal_id)
  {
    e->visited = traversal_id;
    if(visitor)
      fatal(visitor, e, distance, ctx);
  }

  if(travel && e->traveled != traversal_id)
  {
    e->traveled = traversal_id;
    int x = -1;
    while(1)
    {
      x++;
      if((attrs & DIRECTION_OPT) == GRAPH_TRAVERSE_UP)
      {
        if(x == list_size(e->A->up))
          break;
        next_edge = list_get(e->A->up, x);
        next_vertex = next_edge->A;
      }
      else if((attrs & DIRECTION_OPT) == GRAPH_TRAVERSE_DOWN)
      {
        if(x == list_size(e->B->down))
          break;
        next_edge = list_get(e->B->down, x);
        next_vertex = next_edge->B;
      }

      next_travel = 1;
      if(vertex_travel)
        next_travel &= !!(next_vertex->attrs & vertex_travel);
      if(edge_travel)
        next_travel &= !!(next_edge->attrs & edge_travel);

      next_visit = 1;
      if(vertex_visit)
        next_visit &= !!(next_vertex->attrs & vertex_visit);
      if(edge_visit)
        next_visit &= !!(next_edge->attrs & edge_visit);

      if(next_travel || next_visit)
      {
        fatal(explore_edge
          , next_edge
          , visitor
          , vertex_travel
          , vertex_visit
          , edge_travel
          , edge_visit
          , attrs
          , ctx
          , traversal_id
          , distance + 1
          , next_travel
          , next_visit
          , stack
          , stackz
        );
      }
    }
  }

  if((attrs & METHOD_OPT) == GRAPH_TRAVERSE_POST && visit && e->visited != traversal_id)
  {
    e->visited = traversal_id;
    if(visitor)
      fatal(visitor, e, distance, ctx);
  }

  e->guard = 0; // ascend

finally:
  if(XAPI_THROWING(MORIA_CYCLE))
  {
    if((*stackz) < sizeof((*stack)) / sizeof((*stack)[0]))
      (*stack)[(*stackz)++] = e;
  }
coda;
}

static int edge_compare(const void * _X, const void * _Y, void * ctx)
{
  graph * g = ctx;
  const edge * X = list_get(g->edges, *(int*)_X);
  const edge * Y = list_get(g->edges, *(int*)_Y);

  return strcmp(X->A->label, Y->A->label) ?: strcmp(X->B->label, Y->B->label);
}

//
// api
//

API xapi graph_create(graph ** const restrict g)
{
  xproxy(graph_createx, g, 0, 0, 0);
}

API xapi graph_createx(graph ** const restrict g, size_t vsz, void * vertex_value_destroy, void * vertex_value_xdestroy)
{
  enter;

  fatal(xmalloc, g, sizeof(**g));
  (*g)->traversal_id = 1;
  fatal(list_createx, &(*g)->vertices, 0, vertex_xfree, 0);
  fatal(list_createx, &(*g)->edges, wfree, 0, 0);
  (*g)->vsz = vsz;
  (*g)->vertex_value_destroy = vertex_value_destroy;
  (*g)->vertex_value_xdestroy = vertex_value_xdestroy;

  finally : coda;
}

API xapi graph_xfree(graph * const restrict g)
{
  enter;

  if(g)
  {
    if(g->vertex_value_destroy || g->vertex_value_xdestroy)
    {
      int x;
      for(x = 0; x < g->vertices->l; x++)
      {
        vertex * v = list_get(g->vertices, x);
        if(g->vertex_value_destroy)
          g->vertex_value_destroy(v->value);
        else
          fatal(g->vertex_value_xdestroy, v->value);
      }
    }

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

API xapi graph_vertex_create(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs)
{
  enter;

  vertex * lv = 0;
  fatal(vertex_create, &lv, g->vsz, attrs);
  fatal(list_push, g->vertices, (void*)lv);
  *v = lv;
  lv = 0;

finally:
  fatal(vertex_xfree, lv);
coda;
}

API xapi graph_vertex_createw(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs, const char * const restrict label, size_t label_len)
{
  enter;

  vertex * lv = 0;
  fatal(vertex_createw, &lv, g->vsz, attrs, label, label_len);
  fatal(list_push, g->vertices, (void*)lv);
  *v = lv;
  lv = 0;

finally:
  fatal(vertex_xfree, lv);
coda;
}

API xapi graph_connect_edge(graph * const restrict g, vertex * const restrict A, vertex * const restrict B, uint32_t attrs)
{
  enter;

  int x;
  edge * e = 0;
  edge * tmp = 0;
  struct vertex_cmp_context ctx;

  ctx = (typeof(ctx)) { B : B->label, len : B->label_len };
  if(A->down->l == 0)
    ctx.lc = -1;
  else
    list_search(A->down, &ctx, vertex_compare);

  if(ctx.lc)
  {
    fatal(xmalloc, &tmp, sizeof(*tmp));
    tmp->A = A;
    tmp->B = B;
    tmp->attrs = attrs;
    fatal(list_push, g->edges, tmp);
    e = tmp;
    tmp = 0;
    e->edges_index = g->edges->l - 1;

    e->down_index = ctx.lx;
    if(ctx.lc > 0)
      e->down_index++;

    fatal(list_insert, A->down, e->down_index, e);
    for(x = e->down_index + 1; x < A->down->l; x++)
      ((edge*)list_get(A->down, x))->down_index++;

    ctx = (typeof(ctx)){ A : A->label, len : A->label_len };
    if(B->up->l == 0)
      ctx.lc = -1;
    else
      list_search(B->up, &ctx, vertex_compare);

    e->up_index = ctx.lx;
    if(ctx.lc > 0)
      e->up_index++;

    fatal(list_insert, B->up, e->up_index, e);
    for(x = e->up_index + 1; x < B->up->l; x++)
      ((edge*)list_get(B->up, x))->up_index++;
  }

finally:
  wfree(tmp);
coda;
}

API xapi graph_disconnect_edge(graph * const restrict g, vertex * const restrict A, vertex * const restrict B)
{
  enter;

  struct vertex_cmp_context ctx = (typeof(ctx)) { B : B->label, len : B->label_len };
  edge * e = list_search(A->down, &ctx, vertex_compare);

  if(e)
    fatal(edge_disconnect, g, e);

  finally : coda;
}

API xapi graph_say(graph * const restrict g, struct narrator * const restrict N)
{
  enter;

  int * xs = 0;
  int x;

  fatal(xmalloc, &xs, g->edges->l * sizeof(*xs));
  for(x = 0; x < g->edges->l; x++)
    xs[x] = x;

  qsort_r(xs, g->edges->l, sizeof(*xs), edge_compare, g);

  for(x = 0; x < g->edges->l; x++)
  {
    edge * e = list_get(g->edges, xs[x]);
    if(x)
      xsays(" ");
    xsayw(e->A->label, e->A->label_len);
    xsays(":");
    xsayw(e->B->label, e->B->label_len);
  }

finally:
  wfree(xs);
coda;
}

API xapi graph_traverse_vertices(
    graph * const restrict g
  , vertex * const restrict v
  , xapi (* const visitor)(vertex * const restrict, int, void *)
  , int traversal_id
  , uint32_t vertex_travel
  , uint32_t vertex_visit
  , uint32_t edge_travel
  , uint32_t edge_visit
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  vertex * stack[32] = {};
  size_t stackz = 0;

  if(traversal_id != g->traversal_id)
    traversal_id = ++g->traversal_id;

  // defaults
  if(!(attrs & (GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_DOWN)))
    attrs |= GRAPH_TRAVERSE_DOWN;
  if(!(attrs & (GRAPH_TRAVERSE_PRE | GRAPH_TRAVERSE_POST)))
    attrs |= GRAPH_TRAVERSE_POST;

  int travel = 1;
  if(vertex_travel)
    travel &= !!(v->attrs & vertex_travel);

  int visit = 1;
  if(vertex_visit)
    visit &= !!(v->attrs & vertex_visit);

  if(travel || visit)
  {
    fatal(explore_vertex
      , v
      , visitor
      , vertex_travel
      , vertex_visit
      , edge_travel
      , edge_visit
      , attrs
      , ctx
      , traversal_id
      , 0     // distance
      , travel
      , visit
      , &stack
      , &stackz
    );
  }

finally:
  if(XAPI_THROWING(MORIA_CYCLE))
  {
    fatal(narrator_growing_create, &N);

    int x;
    for(x = stackz - 1; x >= 0; x--)
    {
      if(narrator_growing_size(N))
        xsays(" -> ");
      xsayw(stack[x]->label, stack[x]->label_len);
    }

    xapi_infof("nodes", "%zu", stackz);
    xapi_infos("path", narrator_growing_buffer(N));
  }

  fatal(narrator_xfree, N);
coda;
}

API xapi graph_traverse_edges(
    graph * const restrict g
  , edge * const restrict e
  , xapi (* const visitor)(edge * const restrict, int, void *)
  , int traversal_id
  , uint32_t vertex_travel
  , uint32_t vertex_visit
  , uint32_t edge_travel
  , uint32_t edge_visit
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  edge * stack[32] = {};
  size_t stackz = 0;

  if(traversal_id != g->traversal_id)
    traversal_id = ++g->traversal_id;

  // defaults
  if(!(attrs & (GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_DOWN)))
    attrs |= GRAPH_TRAVERSE_DOWN;
  if(!(attrs & (GRAPH_TRAVERSE_PRE | GRAPH_TRAVERSE_POST)))
    attrs |= GRAPH_TRAVERSE_POST;

  int travel = 1;
  if(edge_travel)
    travel &= !!(e->attrs & edge_travel);

  int visit = 1;
  if(edge_visit)
    visit &= !!(e->attrs & edge_visit);

  if(travel || visit)
  {
    fatal(explore_edge
      , e
      , visitor
      , vertex_travel
      , vertex_visit
      , edge_travel
      , edge_visit
      , attrs
      , ctx
      , traversal_id
      , 0     // distance
      , travel
      , visit
      , &stack
      , &stackz
    );
  }

finally:
  if(XAPI_THROWING(MORIA_CYCLE))
  {
    fatal(narrator_growing_create, &N);

    if(stackz)
    {
      xsayw(stack[stackz - 1]->A->label, stack[stackz - 1]->A->label_len);
      xsays(" -> ");
      xsayw(stack[stackz - 1]->B->label, stack[stackz - 1]->B->label_len);
    }

    int x;
    for(x = stackz - 2; x > 0; x--)
    {
      xsays(" -> ");
      xsayw(stack[x]->B->label, stack[x]->B->label_len);
    }

    xapi_infos("path", narrator_growing_buffer(N));
  }

  fatal(narrator_xfree, N);
coda;
}

API int graph_traversal_begin(graph * const restrict g)
{
  return ++g->traversal_id;
}

API list * graph_vertices(graph * g)
{
  return g->vertices;
}

API list * graph_edges(graph * g)
{
  return g->edges;
}
