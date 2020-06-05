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

#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "valyria/list.h"

#include "edge.internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"

#include "macros.h"

//
// public
//

int edge_cmp_up(const edge_t * restrict a, const edge_t * restrict b)
{
  int r;

  uint32_t a_identity;
  uint32_t a_hyper;
  int x;
  const vertex *A;
  const vertex *B;

  a_identity = a->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(a_identity, b->attrs & MORIA_EDGE_IDENTITY)))
    return r;

  a_hyper = a->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(a_hyper, b->attrs & MORIA_EDGE_HYPER)))
    return r;

  if(a_identity)
  {
    A = a->A;
    B = b->A;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len)))
      return r;
  }
  else if(!a_hyper)
  {
    if((r = INTCMP(a->A, b->A)))
      return r;
  }
  else
  {
    if((r = INTCMP(a->Alen, b->Alen)))
      return r;

    for(x = 0; x < a->Alen; x++)
    {
      if((r = INTCMP(a->Alist[x].v, b->Alist[x].v)))
        return r;
    }

    if((r = INTCMP(a->Blen, b->Blen)))
      return r;

    for(x = 0; x < a->Blen; x++)
    {
      if((r = INTCMP(a->Blist[x].v, b->Blist[x].v)))
        return r;
    }
  }

  return 0;
}

int edge_cmp_key_up(void * restrict _key, const rbnode * restrict bn)
{
  int r;

  uint32_t identity;
  uint32_t hyper;
  int x;
  const edge_key * key = _key;
  const edge_t * b;
  const vertex *A;
  const vertex *B;

  b = containerof(bn, edge_t, rbn_up);

  identity = key->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(identity, b->attrs & MORIA_EDGE_IDENTITY)))
    return r;

  hyper = key->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(hyper, b->attrs & MORIA_EDGE_HYPER)))
    return r;

  if(identity)
  {
    A = key->Alist[0];
    B = b->A;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len)))
      return r;
  }
  else if(!hyper)
  {
    if((r = INTCMP(key->Alist[0], b->A)))
      return r;
  }
  else
  {
    if((r = INTCMP(key->Alen, b->Alen)))
      return r;

    for(x = 0; x < key->Alen; x++)
    {
      if((r = INTCMP(key->Alist[x], b->Alist[x].v)))
        return r;
    }

    if((r = INTCMP(key->Blen, b->Blen)))
      return r;

    for(x = 0; x < key->Blen; x++)
    {
      if((r = INTCMP(key->Blist[x], b->Blist[x].v)))
        return r;
    }
  }

  return 0;
}

int edge_cmp_rbn_up(const rbnode * restrict an, const rbnode * restrict bn)
{
  const edge_t * restrict a;
  const edge_t * restrict b;

  if((a = an->ud.p) == 0)
    a = containerof(an, edge_t, rbn_up);

  if((b = bn->ud.p) == 0)
    b = containerof(bn, edge_t, rbn_up);

  return edge_cmp_up(a, b);
}


int edge_cmp_down(const edge_t * restrict a, const edge_t * restrict b)
{
  int r;

  uint32_t a_identity;
  uint32_t a_hyper;
  int x;
  const vertex *A;
  const vertex *B;

  a_identity = a->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(a_identity, b->attrs & MORIA_EDGE_IDENTITY)))
    return r;

  a_hyper = a->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(a_hyper, b->attrs & MORIA_EDGE_HYPER)))
    return r;

  if(a_identity)
  {
    A = a->B;
    B = b->B;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len)))
      return r;
  }
  else if(!a_hyper)
  {
    if((r = INTCMP(a->B, b->B)))
      return r;
  }
  else
  {
    if((r = INTCMP(a->Alen, b->Alen)))
      return r;

    for(x = 0; x < a->Alen; x++)
    {
      if((r = INTCMP(a->Alist[x].v, b->Alist[x].v)))
        return r;
    }

    if((r = INTCMP(a->Blen, b->Blen)))
      return r;

    for(x = 0; x < a->Blen; x++)
    {
      if((r = INTCMP(a->Blist[x].v, b->Blist[x].v)))
        return r;
    }
  }

  return 0;
}

int edge_cmp_key_down(void * _key, const rbnode * restrict bn)
{
  int r;

  uint32_t identity;
  uint32_t hyper;
  int x;
  const edge_key * key = _key;
  const edge_t * b;
  const vertex *A;
  const vertex *B;

  if((b = bn->ud.p) == 0)
    b = containerof(bn, edge_t, rbn_down);

  identity = key->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(identity, b->attrs & MORIA_EDGE_IDENTITY)))
    return r;

  hyper = key->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(hyper, b->attrs & MORIA_EDGE_HYPER)))
    return r;

  if(identity)
  {
    A = key->Blist[0];
    B = b->B;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len)))
      return r;
  }
  else if(!hyper)
  {
    if((r = INTCMP(key->Blist[0], b->B)))
      return r;
  }
  else
  {
    if((r = INTCMP(key->Alen, b->Alen)))
      return r;

    for(x = 0; x < key->Alen; x++)
    {
      if((r = INTCMP(key->Alist[x], b->Alist[x].v)))
        return r;
    }

    if((r = INTCMP(key->Blen, b->Blen)))
      return r;

    for(x = 0; x < key->Blen; x++)
    {
      if((r = INTCMP(key->Blist[x], b->Blist[x].v)))
        return r;
    }
  }

  return 0;
}

int edge_cmp_rbn_down(const rbnode * restrict an, const rbnode * restrict bn)
{
  const edge_t * restrict a;
  const edge_t * restrict b;

  if((a = an->ud.p) == 0)
    a = containerof(an, edge_t, rbn_down);

  if((b = bn->ud.p) == 0)
    b = containerof(bn, edge_t, rbn_down);

  return edge_cmp_down(a, b);
}

//
//
//

xapi edge_alloc(graph * restrict g, edge_t ** restrict re)
{
  enter;

  edge_t * e;

  if((e = llist_shift(&g->edge_freelist, edge_t, graph_lln)) == 0)
  {
    fatal(xmalloc, &e, sizeof(*e) + g->esz);
    graph_edge_init(g, e);
  }
  else
  {
    if(e->attrs & MORIA_EDGE_HYPER)
    {
      wfree(e->Alist);
      wfree(e->Blist);
    }

    memset(e->value, 0, g->esz);
  }
  llist_append(&g->edges, e, graph_lln);

  *re = e;
  e = 0;

  finally : coda;
}

void edge_free(edge_t * restrict e)
{
  if(e && (e->attrs & MORIA_EDGE_HYPER))
  {
    wfree(e->Alist);
    wfree(e->Blist);
  }

  wfree(e);
}

//
// api
//

edge * API edge_between(const vertex * restrict Ax, const vertex * restrict Bx)
{
  vertex_t * A = containerof(Ax, vertex_t, vx);
  vertex_t * B = containerof(Bx, vertex_t, vx);

  rbnode * rbn;
  edge_key key;

  key = (typeof(key)) {
      attrs : MORIA_EDGE_IDENTITY
    , Blist : (vertex*[]) { &B->vx }
    , Blen : 1
  };

  if((rbn = rbtree_lookup_node(&A->down, &key, edge_cmp_key_down)))
    return containerof(rbn, edge, rbn_down);

  key.attrs &= ~MORIA_EDGE_IDENTITY;
  if((rbn = rbtree_lookup_node(&A->down, &key, edge_cmp_key_down)))
    return containerof(rbn, edge, rbn_down);

  return 0;
}

edge * API edge_of(vertex ** restrict Axlist, uint16_t Axlen, vertex ** restrict Bxlist, uint16_t Bxlen)
{
  edge_key key;
  rbnode *rbn;

  if(Axlen == 1 && Bxlen == 1)
    return edge_between(Axlist[0], Bxlist[0]);

  qsort(Axlist, Axlen, sizeof(*Axlist), ptrcmp);
  qsort(Bxlist, Bxlen, sizeof(*Bxlist), ptrcmp);

  key = (typeof(key)) {
      attrs : MORIA_EDGE_HYPER
    , Alist : Axlist
    , Alen : Axlen
    , Blist : Bxlist
    , Blen : Bxlen
  };

  // hyper lookup
  if(Axlen)
    rbn = rbtree_lookup_node(&Axlist[0]->down, &key, edge_cmp_key_down);
  else
    rbn = rbtree_lookup_node(&Bxlist[0]->up, &key, edge_cmp_key_up);

  if(rbn)
    return rbn->ud.p;

  return 0;
}

void API edge_value_set(edge * const restrict ex, graph * restrict g, void * value)
{
  edge_t * e = containerof(ex, edge_t, ex);
  memcpy(e->value, value, g->vsz);
}

void * API edge_value(const edge * const restrict ex)
{
  if(!ex)
    return 0;

  const edge_t * e = containerof(ex, edge_t, ex);

  return (void*)e->value;
}

edge * API edge_containerof(const void * value)
{
  if(!value)
    return 0;

  return (edge*)(value - offsetof(edge_t, value));
}
