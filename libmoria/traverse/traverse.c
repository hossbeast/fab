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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "xapi.h"
#include "types.h"

#include "valyria/list.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "traverse.internal.h"
#include "MORIA.errtab.h"
#include "attr.internal.h"
#include "edge.internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"

#include "attrs.h"
#include "macros.h"
#include "zbuffer.h"

#define TRAVERSAL_ATTRS_TABLES  \
  TRAVERSAL_DIRECTION_TABLE     \
  TRAVERSAL_MODE_TABLE          \

APIDATA attrs32 * traverse_attrs = (attrs32[]) {{
#define TRAVERSAL_ATTR_DEF(x, r, y) + 1
    .num = 0 TRAVERSAL_ATTRS_TABLES
  , .members = (member32[]){
#undef TRAVERSAL_ATTR_DEF
#define TRAVERSAL_ATTR_DEF(x, r, y) { name : #x, value : UINT32_C(y), range : UINT32_C(r) },
TRAVERSAL_ATTRS_TABLES
  }
}};

/* entity with zero-valued index and mask - passing to put/set/del is a no-op */
static entity nullent;

#define GUARD  0
#define VISIT  1
#define TRAVEL 2

static xapi state_assure(traversal_state * restrict state, size_t index)
{
  enter;

  size_t remainder;
  size_t nelem;
  traversal_state *next;
  size_t prev_size = 0;

  while(true)
  {
    if((prev_size + state->size) > index)
      goto XAPI_FINALLY;
    prev_size += state->size;
    if(!state->next)
      break;
    state = state->next;
  }

  /* attach an extra state sufficient for the requested index */
  remainder = index - prev_size;
  nelem = 0;
  while(nelem <= remainder) {
    nelem += (nelem >> 3) + 64;
  }

  fatal(xmalloc, &next, sizeof(*state) + sizeof(*state->bits) * nelem);
  next->size = nelem;
  state->next = next;
  llist_init_node(&next->lln);

  finally : coda;
}

static inline bool set(traversal_state * restrict state, uint8_t bit, const entity * restrict e)
{
  size_t prev_size = 0;

  while(true)
  {
    if(e->index < (prev_size + state->size))
      return (state->bits[e->index - prev_size] & (e->mask << bit)) != 0;
    prev_size += state->size;
    state = state->next;
  }
}

static inline void put(traversal_state * restrict state, uint8_t bit, const entity * restrict e)
{
  size_t prev_size = 0;

  while(true)
  {
    if(e->index < (prev_size + state->size))
    {
      state->bits[e->index - prev_size] |= (e->mask << bit);
      return;
    }
    prev_size += state->size;
    state = state->next;
  }
}

static inline void del(traversal_state * restrict state, uint8_t bit, const entity * restrict e)
{
  size_t prev_size = 0;

  while(true)
  {
    if(e->index < (prev_size + state->size))
    {
      state->bits[e->index - prev_size] &= ~(e->mask << bit);
      return;
    }
    prev_size += state->size;
    state = state->next;
  }
}

static inline vertex_t * follow(vertex_t * v, uint32_t attrs)
{
  if((attrs & MORIA_TRAVERSE_NOFOLLOW) != MORIA_TRAVERSE_NOFOLLOW) {
    while(v->attrs & MORIA_VERTEX_LINK) {
      v = containerof(v->ref, vertex_t, vx);
    }
  }

  return v;
}

static void __attribute__((constructor)) init()
{
  attrs32_init(traverse_attrs);
}

//
// static
//

static inline bool should(uint32_t attrs, uint32_t mask)
{
  if(!mask)
    return true;

  return (attrs & mask) == mask;
}

static xapi explore_edge(
    /*  1 */ edge_t * restrict e
  , /*  2 */ xapi (* vertex_visitor)(vertex* restrict, void *, traversal_mode, int, int * restrict)
  , /*  3 */ xapi (* edge_visitor)(edge * restrict, void *, traversal_mode, int, int * restrict)
  , /*  4 */ const traversal_criteria * restrict criteria
  , /*  5 */ uint32_t attrs
  , /*  6 */ void * ctx
  , /*  7 */ traversal_state * state
  , /*  8 */ int distance
  , /*  9 */ vertex_t * (* restrict vertex_stack)[32]
  , /* 10 */ edge_t * (* restrict edge_stack)[32]
  , /* 11 */ size_t * restrict stackz
)
  __attribute__((nonnull(1, 4, 11)));

static xapi explore_vertex(
    /*  1 */ vertex_t * restrict v
  , /*  2 */ xapi (* vertex_visitor)(vertex * restrict, void *, traversal_mode, int, int * restrict)
  , /*  3 */ xapi (* edge_visitor)(edge* restrict, void *, traversal_mode, int, int * restrict)
  , /*  4 */ const traversal_criteria * restrict criteria
  , /*  5 */ uint32_t attrs
  , /*  6 */ void * ctx
  , /*  7 */ traversal_state * state
  , /*  8 */ int distance
  , /*  9 */ vertex_t * (* restrict vertex_stack)[32]
  , /* 10 */ edge_t * (* restrict edge_stack)[32]
  , /* 11 */ size_t * restrict stackz
)
  __attribute__((nonnull(1, 4, 11)));

static xapi explore_vertex(
    vertex_t * restrict v
  , xapi (* vertex_visitor)(vertex * restrict, void *, traversal_mode, int, int * restrict)
  , xapi (* edge_visitor)(edge* restrict, void *, traversal_mode, int, int * restrict)
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
  , traversal_state * state
  , int distance
  , vertex_t * (* restrict vertex_stack)[32]
  , edge_t * (* restrict edge_stack)[32]
  , size_t * restrict stackz
)
{
  enter;

  edge_t * next_edge = 0;
  bool next_travel;
  bool visit;
  bool travel;
  int result = 0;
  rbnode *rbn;
  entity * ent;
  int next_distance;
  edge_t * identity_edge = 0;

  if(vertex_stack)
  {
    ent = &v->ent;
    next_distance = distance + 1;
  }
  else
  {
    ent = &nullent;
    next_distance = distance;
  }

  travel = true;
  visit = should(v->attrs, criteria->vertex_visit);

  fatal(state_assure, state, ent->index);

  if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance <= criteria->max_depth)
  {
    if(set(state, GUARD, ent))
    {
      // cycle detected ; halt the traversal
      goto XAPI_FINALLY;
    }

    put(state, GUARD, ent);

    if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance >= criteria->min_depth)
    {
      if(visit && (attrs & MORIA_TRAVERSE_PRE) && !set(state, VISIT, ent))
      {
        if(!(attrs & MORIA_TRAVERSE_EXHAUSTIVE)) {
          put(state, VISIT, ent);
        }
        if(vertex_visitor)
        {
          fatal(vertex_visitor, &v->vx, ctx, MORIA_TRAVERSE_PRE, distance, &result);
          if(result == MORIA_TRAVERSE_PRUNE)
            travel = false;
        }
      }
    }
  }

  if(travel && !set(state, TRAVEL, ent))
  {
    if(!(attrs & MORIA_TRAVERSE_EXHAUSTIVE)) {
      put(state, TRAVEL, ent);
    }

    if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
    {
      if((v->up_identity && should(v->up_identity->attrs, criteria->edge_travel)))
      {
        identity_edge = containerof(v->up_identity, edge_t, ex);
      }

      rbn = rbtree_first_post_node(&v->up);
      if(rbn == &rbleaf)
      {
        next_edge = 0;
      }
      else if((next_edge = rbn->ud.p)) { }
      else
      {
        next_edge = containerof(rbn, edge_t, rbn_up);
      }
    }
    else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
    {
      rbn = rbtree_first_post_node(&v->down);
      if(rbn == &rbleaf)
      {
        next_edge = 0;
      }
      else if((next_edge = rbn->ud.p)) { }
      else
      {
        next_edge = containerof(rbn, edge_t, rbn_down);
      }
    }
    while(next_edge)
    {
      next_travel = should(next_edge->attrs, criteria->edge_travel);
      if(next_travel)
      {
        fatal(explore_edge
          , next_edge
          , vertex_visitor
          , edge_visitor
          , criteria
          , attrs
          , ctx
          , state
          , next_distance
          , vertex_stack
          , edge_stack
          , stackz
        );
      }
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
      {
          if((rbn = rbtree_next_post_node(rbn)) == &rbleaf)
          {
            next_edge = 0;
          }
          else if((next_edge = rbn->ud.p))
          {
            /* the next edge is a hyper-edge */
          }
          else
          {
            next_edge = containerof(rbn, edge_t, rbn_up);
          }
      }
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
      {
        if((next_edge->attrs & MORIA_EDGE_HYPER))
        {
          if((rbn = rbtree_next_post_node(rbn)) == &rbleaf)
          {
            next_edge = 0;
          }
          else if((next_edge = rbn->ud.p))
          {
            /* the next edge is a hyper-edge */
          }
          else
          {
            next_edge = containerof(rbn, edge_t, rbn_down);
          }
        }
        else
        {
          next_edge = rbtree_next(next_edge, rbn_down);
        }
      }
    }
    if(identity_edge)
    {
      next_travel = should(identity_edge->attrs, criteria->edge_travel);
      if(next_travel)
      {
        fatal(explore_edge
          , identity_edge
          , vertex_visitor
          , edge_visitor
          , criteria
          , attrs
          , ctx
          , state
          , next_distance
          , vertex_stack
          , edge_stack
          , stackz
        );
      }
    }
  }

  if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance <= criteria->max_depth)
  {
    if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance >= criteria->min_depth)
    {
      if(visit && !!(attrs & MORIA_TRAVERSE_POST) && !set(state, VISIT, ent))
      {
        if(!(attrs & MORIA_TRAVERSE_EXHAUSTIVE)) {
          put(state, VISIT, ent);
        }
        if(vertex_visitor)
        {
          fatal(vertex_visitor, &v->vx, ctx, MORIA_TRAVERSE_POST, distance, &result);
        }
      }
    }

    del(state, GUARD, ent); // ascend
  }

  finally : coda;
}

size_t edge_znload(char * restrict dst, size_t sz, const edge *ex);
API size_t edge_znload(char * restrict dst, size_t sz, const edge *ex)
{
  size_t z = 0;
  int x;
  const edge_t *e = containerof(ex, edge_t, ex);

  z += znloads(dst + z, sz - z, "[up ");
  if(e->attrs & MORIA_EDGE_HYPER)
  {
    for(x = 0; x < e->Alen; x++)
    {
      if(x)
        z += znloads(dst + z, sz - z, ",");
      z += znloadf(dst + z, sz - z, "%s %p", e->Alist[x].v->label, e->Alist[x].v);
    }
  }
  else
  {
    z += znloadf(dst + z, sz - z, "%s %p", e->A->label, e->A);
  }
  z += znloads(dst + z, sz - z, "]");

  z += znloads(dst + z, sz - z, " : ");
  z += znloadf(dst + z, sz - z, "0x%08x", e->attrs);
  z += znloads(dst + z, sz - z, " : ");

  z += znloads(dst + z, sz - z, "[down ");
  if(e->attrs & MORIA_EDGE_HYPER)
  {
    for(x = 0; x < e->Blen; x++)
    {
      if(x)
        z += znloads(dst + z, sz - z, ",");
      z += znloadf(dst + z, sz - z, "%s %p", e->Blist[x].v->label, e->Blist[x].v);
    }
  }
  else
  {
    z += znloadf(dst + z, sz - z, "%s %p", e->B->label, e->B);
  }
  z += znloads(dst + z, sz - z, "]");
  dst[z] = 0;

  return z;
}

static xapi explore_edge(
    edge_t * restrict e
  , xapi (* vertex_visitor)(vertex* restrict, void *, traversal_mode, int, int * restrict)
  , xapi (* edge_visitor)(edge * restrict, void *, traversal_mode, int, int * restrict)
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
  , traversal_state * state
  , int distance
  , vertex_t * (* restrict vertex_stack)[32]
  , edge_t * (* restrict edge_stack)[32]
  , size_t * restrict stackz
)
{
  enter;

  vertex_t * next_vertex = 0;
  bool next_travel;
  bool travel;
  bool visit;
  int result = 0;
  uint16_t vertex_list_pos;
  entity * ent;
  int next_distance;

  if(edge_stack)
  {
    ent = &e->ent;
    next_distance = distance + 1;
  }
  else
  {
    ent = &nullent;
    next_distance = distance;
  }

  travel = true;
  visit = should(e->attrs, criteria->edge_visit);

  fatal(state_assure, state, ent->index);

  if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance <= criteria->max_depth)
  {
    if(set(state, GUARD, ent))
    {
      // cycle detected ; halt the traversal
      goto XAPI_FINALLY;
    }

    put(state, GUARD, ent);

    if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance >= criteria->min_depth)
    {
      if(visit && !!(attrs & MORIA_TRAVERSE_PRE) && !set(state, VISIT, ent))
      {
        if(!(attrs & MORIA_TRAVERSE_EXHAUSTIVE)) {
          put(state, VISIT, ent);
        }
        if(edge_visitor)
        {
          fatal(edge_visitor, &e->ex, ctx, MORIA_TRAVERSE_PRE, distance, &result);
          if(result == MORIA_TRAVERSE_PRUNE)
            travel = false;
        }
      }
    }
  }

  if(travel && !set(state, TRAVEL, ent))
  {
    if(!(attrs & MORIA_TRAVERSE_EXHAUSTIVE)) {
      put(state, TRAVEL, ent);
    }

    if(e->attrs & MORIA_EDGE_HYPER)
      vertex_list_pos = 0;

    if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
    {
      if(e->attrs & MORIA_EDGE_HYPER)
      {
        if(e->Alen)
          next_vertex = containerof(e->Alist[vertex_list_pos++].v, vertex_t, vx);
        else
          next_vertex = 0;
      }
      else
      {
        next_vertex = containerof(e->A, vertex_t, vx);
      }
    }
    else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
    {
      if(e->attrs & MORIA_EDGE_HYPER)
      {
        if(e->Blen)
          next_vertex = containerof(e->Blist[vertex_list_pos++].v, vertex_t, vx);
        else
          next_vertex = 0;
      }
      else
      {
        next_vertex = containerof(e->B, vertex_t, vx);
      }
    }
    while(next_vertex)
    {
      next_vertex = follow(next_vertex, attrs);
      next_travel = should(next_vertex->attrs, criteria->vertex_travel);

      if(next_travel)
      {
        fatal(explore_vertex
          , next_vertex
          , vertex_visitor
          , edge_visitor
          , criteria
          , attrs
          , ctx
          , state
          , next_distance
          , vertex_stack
          , edge_stack
          , stackz
        );
      }
      next_vertex = 0;
      if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
      {
        if((e->attrs & MORIA_EDGE_HYPER) && vertex_list_pos < e->Alen)
        {
          next_vertex = containerof(e->Alist[vertex_list_pos++].v, vertex_t, vx);
        }
      }
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
      {
        if((e->attrs & MORIA_EDGE_HYPER) && vertex_list_pos < e->Blen)
        {
          next_vertex = containerof(e->Blist[vertex_list_pos++].v, vertex_t, vx);
        }
      }
    }
  }

  if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance <= criteria->max_depth)
  {
    if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance >= criteria->min_depth)
    {
      if(visit && !!(attrs & MORIA_TRAVERSE_POST) && !set(state, VISIT, ent))
      {
        if(!(attrs & MORIA_TRAVERSE_EXHAUSTIVE)) {
          put(state, VISIT, ent);
        }
        if(edge_visitor)
        {
          fatal(edge_visitor, &e->ex, ctx, MORIA_TRAVERSE_POST, distance, &result);
        }
      }
    }

    del(state, GUARD, ent);
  }

  finally : coda;
}

static xapi extremum_visitor(vertex * restrict v, void * ctx, traversal_mode mode, int distance, int * restrict r)
{
  enter;

  bool * extreme = ctx;

  if(distance == 0)
  {
    *extreme = true;
  }
  else if(distance > 0)
  {
    *extreme = false;
    *r = MORIA_TRAVERSE_PRUNE;
  }

  finally : coda;
}

/*
 * check whether this vertex occupies a local extremum
 *
 * v   - vertex
 */
static xapi vertex_is_extremum(graph * const restrict g, vertex_t * restrict v, const traversal_criteria * restrict criteria, uint32_t attrs, bool * restrict rv)
{
  enter;

  narrator * N = 0;
  vertex_t * stack[32] = {};
  size_t stackz = 0;
  vertex_traversal_state * state = 0;
  bool travel;
  bool extreme = false;

  travel = should(v->attrs, criteria->vertex_travel);
  if(travel)
  {
    fatal(graph_vertex_traversal_begin, g, &state);
    fatal(explore_vertex
      , v
      , extremum_visitor
      , 0
      , criteria
      , attrs
      , &extreme
      , &state->st
      , 0
      , &stack
      , 0
      , &stackz
    );
  }

  *rv = extreme;

finally:
  graph_vertex_traversal_end(g, state);
  fatal(narrator_xfree, N);
coda;
}

static xapi traversal_begin(graph * const restrict g, size_t max_index, traversal_state ** restrict statep)
{
  enter;

  size_t osize;
  size_t size;
  size_t nelem;

  traversal_state * state = 0;

  if(!llist_empty(&g->states))
  {
    state = llist_shift(&g->states, traversal_state, lln);
  }

  if(!state || state->size <= max_index)
  {
    osize = 0;
    if(state)
      osize = sizeof(*state) + (sizeof(*state->bits) * state->size);

    nelem = 0;
    if(state)
      nelem = state->size;
    while(nelem <= max_index) {
      nelem += (nelem >> 3) + 64;
    }
    size = sizeof(*state) + (sizeof(*state->bits) * nelem);

    fatal(xrealloc, &state, 1, size, osize);

    llist_init_node(&state->lln);
    state->size = nelem;

  }

  memset(state->bits, 0, sizeof(*state->bits) * (max_index + 1));
  *statep = state;

  finally : coda;
}

//
// api
//

API xapi graph_traverse_vertices(
    graph * const restrict g
  , vertex * const restrict vx
  , xapi (* visitor)(vertex * restrict, void *, traversal_mode, int, int * restrict)
  , vertex_traversal_state * state
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  vertex_t * stack[32] = {};
  size_t stackz = 0;
  traversal_criteria local_criteria;
  vertex_traversal_state * local_state = 0;
  bool travel;

  vertex_t * v = containerof(vx, vertex_t, vx);

  v = follow(v, attrs);

  if(!state)
  {
    fatal(graph_vertex_traversal_begin, g, &local_state);
    state = local_state;
  }

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  travel = should(v->attrs, criteria->vertex_travel);
  if(travel)
  {
    fatal(explore_vertex
      , v
      , visitor   // vertex visitor
      , 0
      , criteria
      , attrs
      , ctx
      , &state->st
      , 0         // distance
      , &stack    // vertex stack
      , 0
      , &stackz
    );
  }

finally:
  graph_vertex_traversal_end(g, local_state);
  fatal(narrator_xfree, N);
coda;
}

API xapi graph_traverse_vertex_edges(
    graph * const restrict g
  , vertex * const restrict vx
  , xapi (* visitor)(edge * restrict, void *, traversal_mode, int, int * restrict)
  , edge_traversal_state * state
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  edge_t * stack[32] = {};
  size_t stackz = 0;
  traversal_criteria local_criteria;
  edge_traversal_state * local_state = 0;

  vertex_t * v = containerof(vx, vertex_t, vx);
  v = follow(v, attrs);

  if(!state)
  {
    fatal(graph_edge_traversal_begin, g, &local_state);
    state = local_state;
  }

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  fatal(explore_vertex
    , v
    , 0
    , visitor   // edge visitor
    , criteria
    , attrs
    , ctx
    , &state->st
    , 0         // distance
    , 0
    , &stack    // edge stack
    , &stackz
  );

finally:
  graph_edge_traversal_end(g, local_state);
  fatal(narrator_xfree, N);
coda;
}

API xapi graph_traverse_edges(
    graph * const restrict g
  , edge * const restrict ex
  , xapi (* visitor)(edge * restrict, void *, traversal_mode, int, int * restrict)
  , edge_traversal_state * state
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  narrator * N = 0;
  edge_t * stack[32] = {};
  size_t stackz = 0;
  traversal_criteria local_criteria;
  edge_traversal_state * local_state = 0;
  bool travel;
  edge_t * e = containerof(ex, edge_t, ex);

  if(state == 0)
  {
    fatal(graph_edge_traversal_begin, g, &local_state);
    state = local_state;
  }

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  travel = should(e->attrs, criteria->edge_travel);

  if(travel)
  {
    fatal(explore_edge
      , e
      , 0
      , visitor   // edge visitor
      , criteria
      , attrs
      , ctx
      , &state->st
      , 0         // distance
      , 0
      , &stack
      , &stackz
    );
  }

finally:
  graph_edge_traversal_end(g, local_state);
  fatal(narrator_xfree, N);
coda;
}

API xapi graph_traverse_vertices_all(
    graph * const restrict g
  , xapi (* visitor)(vertex * restrict, void *, traversal_mode, int, int * restrict)
  , const traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  enter;

  vertex_traversal_state * state = 0;
  vertex_t *v;
  bool extremum;
  uint32_t extremum_attrs;

  fatal(graph_vertex_traversal_begin, g, &state);

  extremum_attrs = attrs & ~DIRECTION_OPT;
  if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
    extremum_attrs |= MORIA_TRAVERSE_DOWN;
  else
    extremum_attrs |= MORIA_TRAVERSE_UP;

  // traverse from vertices situated at local extrema
  llist_foreach(&g->vertices, v, graph_lln) {
    if(set(&state->st, VISIT, &v->ent))
      continue;

    fatal(vertex_is_extremum, g, v, criteria, extremum_attrs, &extremum);
    if(extremum)
    {
      fatal(graph_traverse_vertices, g, &v->vx, visitor, state, criteria, attrs, ctx);
    }
  }

  // isolated vertices
  llist_foreach(&g->vertices, v, graph_lln) {
    if(!set(&state->st, VISIT, &v->ent)) {
      fatal(graph_traverse_vertices, g, &v->vx, visitor, state, criteria, attrs, ctx);
    }
  }

finally:
  graph_vertex_traversal_end(g, state);
coda;
}

API xapi graph_vertex_traversal_begin(graph * const restrict g, vertex_traversal_state ** restrict vstp)
{
  enter;

  traversal_state *statep;

  fatal(traversal_begin, g, g->vertex_index, &statep);

  *vstp = containerof(statep, vertex_traversal_state, st);

  finally : coda;
}

API xapi graph_edge_traversal_begin(graph * const restrict g, edge_traversal_state ** restrict estp)
{
  enter;

  traversal_state *statep;

  fatal(traversal_begin, g, g->edge_index, &statep);

  *estp = containerof(statep, edge_traversal_state, st);

  finally : coda;
}

static void traversal_state_reclaim(graph * restrict g, traversal_state * restrict state)
{
  traversal_state *next, *T;

  llist_append(&g->states, state, lln);

  /* recover any extra states attached to this one from a previous traversal */
  next = state;
  while((T = next->next))
  {
    next->next = 0;
    llist_append(&g->states, T, lln);
    next = T;
  }
}

API void graph_vertex_traversal_end(graph * const restrict g, vertex_traversal_state * restrict state)
{
  if(!state)
    return;

  traversal_state_reclaim(g, &state->st);
}

API void graph_edge_traversal_end(graph * const restrict g, edge_traversal_state * restrict state)
{
  if(!state)
    return;

  traversal_state_reclaim(g, &state->st);
}

API bool graph_traversal_vertex_visited(const graph * const restrict g, const vertex * const restrict vx, vertex_traversal_state * restrict state)
{
  const vertex_t * v = containerof(vx, vertex_t, vx);

  return set(&state->st, VISIT, &v->ent);
}

API bool graph_traversal_edge_visited(const graph * const restrict g, const edge * const restrict ex, edge_traversal_state * restrict state)
{
  const edge_t * e = containerof(ex, edge_t, ex);

  return set(&state->st, VISIT, &e->ent);
}
