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

#include "xlinux/xstdlib.h"
#include "common/attrs.h"

#include "traverse.internal.h"
#include "attr.h"
#include "moria.h"
#include "probes.h"

#define GUARD  0
#define VISIT  1
#define TRAVEL 2

attrs32 * APIDATA moria_traverse_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, r, y) + 1
    .num = 0
MORIA_TRAVERSAL_DIRECTION_TABLE
MORIA_TRAVERSAL_MODE_TABLE

  , .members = (member32[]){
#undef DEF
#define DEF(x, r, y) { name : #x, value : UINT32_C(y), range : UINT32_C(r) },
MORIA_TRAVERSAL_DIRECTION_TABLE
MORIA_TRAVERSAL_MODE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(moria_traverse_attrs);
}

/* moria_traversible with zero-valued index and mask - passing to put/set/del is a no-op */
static moria_traversible null_traversible;

static void state_assure(traversal_state * restrict state, size_t index)
{
  size_t remainder;
  size_t nelem;
  traversal_state *next;
  size_t prev_size = 0;

  while(true)
  {
    if((prev_size + state->size) > index)
      return;
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

  xmalloc(&next, sizeof(*state) + sizeof(*state->bits) * nelem);
  next->size = nelem;
  state->next = next;
  llist_init_node(&next->lln);
}

static inline bool set(traversal_state * restrict state, uint8_t bit, const moria_traversible * restrict e)
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

static inline void put(traversal_state * restrict state, uint8_t bit, const moria_traversible * restrict e)
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

static inline void del(traversal_state * restrict state, uint8_t bit, const moria_traversible * restrict e)
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

static inline moria_vertex * follow(moria_vertex * v, uint32_t attrs)
{
  if((attrs & MORIA_TRAVERSE_NOFOLLOW) != MORIA_TRAVERSE_NOFOLLOW) {
    while(v->attrs & MORIA_VERTEX_LINK) {
      v = v->ref;
    }
  }

  return v;
}

//
// static
//

static inline bool should(uint32_t attrs, uint32_t mask)
{
  if(!mask) {
    return true;
  }

  return (attrs & mask) == mask;
}

static void explore_edge(
    /*  1 */ moria_edge * restrict e
  , /*  2 */ void (* vertex_visitor)(moria_vertex * restrict, void *, moria_traversal_mode, int, int * restrict)
  , /*  3 */ void (* edge_visitor)(moria_edge * restrict, void *, moria_traversal_mode, int, int * restrict)
  , /*  4 */ const moria_traversal_criteria * restrict criteria
  , /*  5 */ uint32_t attrs
  , /*  6 */ void * ctx
  , /*  7 */ traversal_state * state
  , /*  8 */ int distance
  , /*  9 */ moria_vertex * (* restrict vertex_stack)[32]
  , /* 10 */ moria_edge * (* restrict edge_stack)[32]
  , /* 11 */ size_t * restrict stackz
)
  __attribute__((nonnull(1, 4, 11)));

static void explore_vertex(
    /*  1 */ moria_vertex * restrict v
  , /*  2 */ void (* vertex_visitor)(moria_vertex * restrict, void *, moria_traversal_mode, int, int * restrict)
  , /*  3 */ void (* edge_visitor)(moria_edge * restrict, void *, moria_traversal_mode, int, int * restrict)
  , /*  4 */ const moria_traversal_criteria * restrict criteria
  , /*  5 */ uint32_t attrs
  , /*  6 */ void * ctx
  , /*  7 */ traversal_state * state
  , /*  8 */ int distance
  , /*  9 */ moria_vertex * (* restrict vertex_stack)[32]
  , /* 10 */ moria_edge * (* restrict edge_stack)[32]
  , /* 11 */ size_t * restrict stackz
)
  __attribute__((nonnull(1, 4, 11)));

static void explore_vertex(
    moria_vertex * restrict v
  , void (* vertex_visitor)(moria_vertex * restrict, void *, moria_traversal_mode, int, int * restrict)
  , void (* edge_visitor)(moria_edge * restrict, void *, moria_traversal_mode, int, int * restrict)
  , const moria_traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
  , traversal_state * state
  , int distance
  , moria_vertex * (* restrict vertex_stack)[32]
  , moria_edge * (* restrict edge_stack)[32]
  , size_t * restrict stackz
)
{
  moria_edge * next_edge = 0;
  bool next_travel;
  bool visit;
  bool travel;
  int result = 0;
  rbnode *rbn;
  moria_traversible * ent;
  int next_distance;
  moria_edge * identity_edge = 0;

  if(vertex_stack)
  {
    ent = &v->ent;
    next_distance = distance + 1;
  }
  else
  {
    ent = &null_traversible;
    next_distance = distance;
  }

  travel = true;
  visit = should(v->attrs, criteria->vertex_visit);

  if(moria_explore_vertex_semaphore) {
    explore_vertex_probe(v, distance, travel, visit);
  }

  state_assure(state, ent->index);

  if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance <= criteria->max_depth)
  {
    if(set(state, GUARD, ent))
    {
      // cycle detected ; halt the traversal
      return;
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
          vertex_visitor(v, ctx, MORIA_TRAVERSE_PRE, distance, &result);
          if(result == MORIA_TRAVERSE_PRUNE) {
            travel = false;
          }
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
        identity_edge = v->up_identity;
      }

      rbn = rbtree_first_post_node(&v->up);
      if(rbn == &rbleaf)
      {
        next_edge = 0;
      }
      else if((next_edge = rbn->ud.p)) { }
      else
      {
        next_edge = containerof(rbn, moria_edge, rbn_up);
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
        next_edge = containerof(rbn, moria_edge, rbn_down);
      }
    }
    while(next_edge)
    {
      next_travel = should(next_edge->attrs, criteria->edge_travel);
      if(next_travel)
      {
        explore_edge(
            next_edge
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
          /* the next edge is a hyper-moria_edge */
        }
        else
        {
          next_edge = containerof(rbn, moria_edge, rbn_up);
        }
      }
      else
      {
        RUNTIME_ASSERT((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN);

        if((rbn = rbtree_next_post_node(rbn)) == &rbleaf)
        {
          next_edge = 0;
        }
        else if((next_edge = rbn->ud.p))
        {
          /* the next edge is a hyper-moria_edge */
        }
        else
        {
          next_edge = containerof(rbn, moria_edge, rbn_down);
        }
      }
    }
    if(identity_edge)
    {
      next_travel = should(identity_edge->attrs, criteria->edge_travel);
      if(next_travel)
      {
        explore_edge(
            identity_edge
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
          vertex_visitor(v, ctx, MORIA_TRAVERSE_POST, distance, &result);
        }
      }
    }

    del(state, GUARD, ent); // ascend
  }
}

static void explore_edge(
    moria_edge * restrict e
  , void (* vertex_visitor)(moria_vertex * restrict, void *, moria_traversal_mode, int, int * restrict)
  , void (* edge_visitor)(moria_edge * restrict, void *, moria_traversal_mode, int, int * restrict)
  , const moria_traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
  , traversal_state * state
  , int distance
  , moria_vertex * (* restrict vertex_stack)[32]
  , moria_edge * (* restrict edge_stack)[32]
  , size_t * restrict stackz
)
{
  moria_vertex * next_vertex = 0;
  bool next_travel;
  bool travel;
  bool visit;
  int result = 0;
  uint16_t vertex_list_pos;
  moria_traversible * ent;
  int next_distance;

  if(edge_stack)
  {
    ent = &e->ent;
    next_distance = distance + 1;
  }
  else
  {
    ent = &null_traversible;
    next_distance = distance;
  }

  travel = true;
  visit = should(e->attrs, criteria->edge_visit);

  if(moria_explore_edge_semaphore) {
    explore_edge_probe(e, distance, travel, visit);
  }

  state_assure(state, ent->index);

  if(!(attrs & MORIA_TRAVERSE_DEPTH) || distance <= criteria->max_depth)
  {
    if(set(state, GUARD, ent))
    {
      // cycle detected ; halt the traversal
      return;
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
          edge_visitor(e, ctx, MORIA_TRAVERSE_PRE, distance, &result);
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
          next_vertex = e->Alist[vertex_list_pos++].v;
        else
          next_vertex = 0;
      }
      else
      {
        next_vertex = e->A;
      }
    }
    else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
    {
      if(e->attrs & MORIA_EDGE_HYPER)
      {
        if(e->Blen)
          next_vertex = e->Blist[vertex_list_pos++].v;
        else
          next_vertex = 0;
      }
      else
      {
        next_vertex = e->B;
      }
    }
    while(next_vertex)
    {
      next_vertex = follow(next_vertex, attrs);
      next_travel = should(next_vertex->attrs, criteria->vertex_travel);
      if(next_travel)
      {
        explore_vertex(
            next_vertex
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
          next_vertex = e->Alist[vertex_list_pos++].v;
        }
      }
      else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
      {
        if((e->attrs & MORIA_EDGE_HYPER) && vertex_list_pos < e->Blen)
        {
          next_vertex = e->Blist[vertex_list_pos++].v;
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
          edge_visitor(e, ctx, MORIA_TRAVERSE_POST, distance, &result);
        }
      }
    }

    del(state, GUARD, ent);
  }
}

static void traversal_begin(moria_graph * const restrict g, size_t max_index, traversal_state ** restrict statep)
{
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

    xrealloc(&state, 1, size, osize);

    llist_init_node(&state->lln);
    state->size = nelem;

  }

  memset(state->bits, 0, sizeof(*state->bits) * (max_index + 1));
  *statep = state;
}

static void traversal_state_reclaim(moria_graph * restrict g, traversal_state * restrict state)
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

//
// api
//

void API moria_traverse_vertices(
    moria_graph * const restrict g
  , moria_vertex * const restrict vx
  , void (* visitor)(moria_vertex * restrict, void *, moria_traversal_mode, int, int * restrict)
  , moria_vertex_traversal_state * state
  , const moria_traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  moria_vertex * stack[32] = {};
  size_t stackz = 0;
  moria_traversal_criteria local_criteria;
  moria_vertex_traversal_state * local_state = 0;
  bool travel;

  RUNTIME_ASSERT(attrs & (MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_POST));

  moria_vertex * v = vx;
  v = follow(v, attrs);

  if(!state)
  {
    moria_vertex_traversal_begin(g, &local_state);
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
    explore_vertex(
        v
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

  moria_vertex_traversal_end(g, local_state);
}

void API moria_traverse_vertex_edges(
    moria_graph * const restrict g
  , moria_vertex * const restrict vx
  , void (* visitor)(moria_edge * restrict, void *, moria_traversal_mode, int, int * restrict)
  , moria_edge_traversal_state * state
  , const moria_traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  moria_edge * stack[32] = {};
  size_t stackz = 0;
  moria_traversal_criteria local_criteria;
  moria_edge_traversal_state * local_state = 0;

  RUNTIME_ASSERT(attrs & (MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_POST));

  moria_vertex * v = vx;
  v = follow(v, attrs);

  if(!state)
  {
    moria_edge_traversal_begin(g, &local_state);
    state = local_state;
  }

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  explore_vertex(
      v
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

  moria_edge_traversal_end(g, local_state);
}

void API moria_traverse_edges(
    moria_graph * const restrict g
  , moria_edge * const restrict ex
  , void (* visitor)(moria_edge * restrict, void *, moria_traversal_mode, int, int * restrict)
  , moria_edge_traversal_state * state
  , const moria_traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  moria_edge * stack[32] = {};
  size_t stackz = 0;
  moria_traversal_criteria local_criteria;
  moria_edge_traversal_state * local_state = 0;
  bool travel;
  moria_edge * e = ex;

  RUNTIME_ASSERT(attrs & (MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_POST));

  if(state == 0)
  {
    moria_edge_traversal_begin(g, &local_state);
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
    explore_edge(
        e
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

  moria_edge_traversal_end(g, local_state);
}

void API moria_traverse_edge_vertices(
    moria_graph * const restrict g
  , moria_edge * const restrict ex
  , void (* visitor)(moria_vertex * restrict, void *, moria_traversal_mode, int, int * restrict)
  , moria_vertex_traversal_state * state
  , const moria_traversal_criteria * restrict criteria
  , uint32_t attrs
  , void * ctx
)
{
  moria_vertex * stack[32] = {};
  size_t stackz = 0;
  moria_traversal_criteria local_criteria;
  moria_vertex_traversal_state * local_state = 0;
  moria_edge * e = ex;

  RUNTIME_ASSERT(attrs & (MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_POST));

  if(!state)
  {
    moria_vertex_traversal_begin(g, &local_state);
    state = local_state;
  }

  if(!criteria)
  {
    memset(&local_criteria, 0, sizeof(local_criteria));
    criteria = &local_criteria;
  }

  explore_edge(
      e
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

  moria_vertex_traversal_end(g, local_state);
}

void API moria_vertex_traversal_begin(moria_graph * const restrict g, moria_vertex_traversal_state ** restrict vstp)
{
  traversal_state *statep;

  traversal_begin(g, g->vertex_index, &statep);

  *vstp = containerof(statep, moria_vertex_traversal_state, st);
}

void API moria_edge_traversal_begin(moria_graph * const restrict g, moria_edge_traversal_state ** restrict estp)
{
  traversal_state *statep;

  traversal_begin(g, g->edge_index, &statep);

  *estp = containerof(statep, moria_edge_traversal_state, st);
}

void API moria_vertex_traversal_end(moria_graph * const restrict g, moria_vertex_traversal_state * restrict state)
{
  if(!state)
    return;

  traversal_state_reclaim(g, &state->st);
}

void API moria_edge_traversal_end(moria_graph * const restrict g, moria_edge_traversal_state * restrict state)
{
  if(!state)
    return;

  traversal_state_reclaim(g, &state->st);
}

bool API moria_traversal_vertex_visited(const moria_graph * const restrict g, const moria_vertex * const restrict vx, moria_vertex_traversal_state * restrict state)
{
  const moria_vertex * v = vx;

  return set(&state->st, VISIT, &v->ent);
}

bool API moria_traversal_edge_visited(const moria_graph * const restrict g, const moria_edge * const restrict ex, moria_edge_traversal_state * restrict state)
{
  const moria_edge * e = ex;

  return set(&state->st, VISIT, &e->ent);
}

/* visitors */

void API moria_vertex_count(struct moria_vertex * restrict v, void * ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  uint32_t *c = ctx;
  (*c)++;
}

void API moria_vertex_count_once(struct moria_vertex * restrict v, void * ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  uint32_t *c = ctx;
  (*c)++;

  *result = MORIA_TRAVERSE_PRUNE;
}

void API moria_edge_count(struct moria_edge * restrict v, void * ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  uint32_t *c = ctx;
  (*c)++;
}

void API moria_edge_count_once(struct moria_edge * restrict v, void * ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  uint32_t *c = ctx;
  (*c)++;

  *result = MORIA_TRAVERSE_PRUNE;
}
