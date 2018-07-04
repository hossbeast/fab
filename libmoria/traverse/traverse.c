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
#include "types.h"

#include "valyria/list.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "internal.h"
#include "traverse.internal.h"
#include "MORIA.errtab.h"
#include "attr.internal.h"
#include "edge.internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"

//
// static
//

static xapi __attribute__((nonnull(1, 3, 10, 11))) explore_vertex(
    /*  1 */ vertex * restrict v
  , /*  2 */ xapi (* visitor)(vertex * restrict, int, void *)
  , /*  3 */ const traversal_criteria * restrict criteria
  , /*  4 */ uint32_t attrs
  , /*  5 */ void * ctx
  , /*  6 */ int traversal_id
  , /*  7 */ int distance
  , /*  8 */ bool travel
  , /*  9 */ bool visit
  , /* 10 */ vertex * (* restrict stack)[32]
  , /* 11 */ size_t * restrict stackz
)
{
  enter;

  edge * next_edge = 0;
  vertex * next_vertex = 0;
  bool next_travel;
  bool next_visit;

  if(v->guard)  // cycle detected
    fail(MORIA_CYCLE);

  v->guard = 1; // descend

  if((attrs & METHOD_OPT) == MORIA_TRAVERSE_PRE && visit && v->visited != traversal_id)
  {
    if(visitor)
      fatal(visitor, v, distance, ctx);
    v->visited = traversal_id;
  }

  if(travel && v->traveled != traversal_id)
  {
    v->traveled = traversal_id;
    int x = -1;
    while(1)
    {
      x++;
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
      {
        if(x == list_size(v->up))
          break;
        next_edge = list_get(v->up, x);
        next_vertex = next_edge->A;
      }
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
      {
        if(x == list_size(v->down))
          break;
        next_edge = list_get(v->down, x);
        next_vertex = next_edge->B;
      }

      next_travel = true;
      if(criteria->vertex_travel)
        next_travel &= !!(next_vertex->attrs & criteria->vertex_travel);
      if(criteria->edge_travel)
        next_travel &= !!(next_edge->attrs & criteria->edge_travel);

      next_visit = true;
      if(criteria->vertex_visit)
        next_visit &= !!(next_vertex->attrs & criteria->vertex_visit);
      if(criteria->edge_visit)
        next_visit &= !!(next_edge->attrs & criteria->edge_visit);

      if(next_travel || next_visit)
      {
        fatal(explore_vertex
          , next_vertex
          , visitor
          , criteria
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

  if((attrs & METHOD_OPT) == MORIA_TRAVERSE_POST && visit && v->visited != traversal_id)
  {
    if(visitor)
      fatal(visitor, v, distance, ctx);
    v->visited = traversal_id;
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

static xapi __attribute__((nonnull(1, 3, 10, 11))) explore_edge(
    /*  1 */ edge * restrict e
  , /*  2 */ xapi (* visitor)(edge * restrict, int, void *)
  , /*  3 */ const traversal_criteria * restrict criteria
  , /*  4 */ uint32_t attrs
  , /*  5 */ void * ctx
  , /*  6 */ int traversal_id
  , /*  7 */ int distance
  , /*  8 */ bool travel
  , /*  9 */ bool visit
  , /* 10 */ edge * (* restrict stack)[32]
  , /* 11 */ size_t * restrict stackz
)
{
  enter;

  edge * next_edge = 0;
  vertex * next_vertex = 0;
  bool next_travel;
  bool next_visit;

  if(e->guard)  // cycle detected
    fail(MORIA_CYCLE);

  e->guard = 1; // descend

  if((attrs & METHOD_OPT) == MORIA_TRAVERSE_PRE && visit && e->visited != traversal_id)
  {
    if(visitor)
      fatal(visitor, e, distance, ctx);
    e->visited = traversal_id;
  }

  if(travel && e->traveled != traversal_id)
  {
    e->traveled = traversal_id;
    int x = -1;
    while(1)
    {
      x++;
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
      {
        if(x == list_size(e->A->up))
          break;
        next_edge = list_get(e->A->up, x);
        next_vertex = next_edge->A;
      }
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
      {
        if(x == list_size(e->B->down))
          break;
        next_edge = list_get(e->B->down, x);
        next_vertex = next_edge->B;
      }

      next_travel = true;
      if(criteria->vertex_travel)
        next_travel &= !!(next_vertex->attrs & criteria->vertex_travel);
      if(criteria->edge_travel)
        next_travel &= !!(next_edge->attrs & criteria->edge_travel);

      next_visit = true;
      if(criteria->vertex_visit)
        next_visit &= !!(next_vertex->attrs & criteria->vertex_visit);
      if(criteria->edge_visit)
        next_visit &= !!(next_edge->attrs & criteria->edge_visit);

      if(next_travel || next_visit)
      {
        fatal(explore_edge
          , next_edge
          , visitor
          , criteria
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

  if((attrs & METHOD_OPT) == MORIA_TRAVERSE_POST && visit && e->visited != traversal_id)
  {
    if(visitor)
      fatal(visitor, e, distance, ctx);
    e->visited = traversal_id;
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

//
// api
//

API xapi graph_traverse_vertices(
    graph * const restrict g
  , vertex * const restrict v
  , xapi (* const visitor)(vertex * const restrict, int, void *)
  , int traversal_id
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  vertex * stack[32] = {};
  size_t stackz = 0;
  traversal_criteria local_criteria;

  if(traversal_id != g->traversal_id)
    traversal_id = ++g->traversal_id;

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  bool travel = true;
  if(criteria->vertex_travel)
    travel &= !!(v->attrs & criteria->vertex_travel);

  bool visit = true;
  if(criteria->vertex_visit)
    visit &= !!(v->attrs & criteria->vertex_visit);

  if(travel || visit)
  {
    fatal(explore_vertex
      , v
      , visitor
      , criteria
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
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  edge * stack[32] = {};
  size_t stackz = 0;
  traversal_criteria local_criteria;

  if(traversal_id != g->traversal_id)
    traversal_id = ++g->traversal_id;

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  bool travel = true;
  if(criteria->edge_travel)
    travel &= !!(e->attrs & criteria->edge_travel);

  bool visit = true;
  if(criteria->edge_visit)
    visit &= !!(e->attrs & criteria->edge_visit);

  if(travel || visit)
  {
    fatal(explore_edge
      , e
      , visitor
      , criteria
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

API xapi graph_traverse_vertices_all(
    graph * const restrict g
  , xapi (* const visitor)(vertex * const restrict, int, void *)
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  int traversal_id = graph_traversal_begin(g);

  int x;
  for(x = 0; x < g->vertices->l; x++)
  {
    vertex * v = list_get(g->vertices, x);

    // check whether this node occupies a local maximum
    int y = -1;
    while(1)
    {
      y++;
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP && y == v->down->l)
        break;
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN && y == v->up->l)
        break;

      edge * e = 0;
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
        e = list_get(v->down, y);
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
        e = list_get(v->up, y);

      vertex * nv = 0;
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
        nv = e->B;
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
        nv = e->A;

      bool next_travel = true;
      if(criteria && criteria->vertex_travel)
        next_travel &= !!(nv->attrs & criteria->vertex_travel);
      if(criteria && criteria->edge_travel)
        next_travel &= !!(e->attrs & criteria->edge_travel);

      if(!next_travel)
        break;

      bool next_visit = true;
      if(criteria && criteria->vertex_visit)
        next_visit &= !!(nv->attrs & criteria->vertex_visit);
      if(criteria && criteria->edge_visit)
        next_visit &= !!(e->attrs & criteria->edge_visit);

      if(!next_visit)
        break;
    }

    bool maximum = true;
    if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP && y < v->down->l)
      maximum = false;
    if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN && y < v->up->l)
      maximum = false;
    
    if(maximum)
      fatal(graph_traverse_vertices, g, v, visitor, traversal_id, criteria, attrs, ctx);
  }

  // isolated vertices
  for(x = 0; x < g->vertices->l; x++)
  {
    vertex * v = list_get(g->vertices, x);

    if(v->traveled != traversal_id)
      fatal(graph_traverse_vertices, g, v, visitor, traversal_id, criteria, attrs, ctx);
  }

  finally : coda;
}

API int graph_traversal_begin(graph * const restrict g)
{
  return ++g->traversal_id;
}
