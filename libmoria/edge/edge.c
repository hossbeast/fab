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

#include <stdlib.h>

#include "moria.h"

#include "edge.internal.h"
#include "graph.internal.h"

#include "macros.h"

//
// public
//

int edge_cmp_up(const moria_edge * restrict a, const moria_edge * restrict b)
{
  int r;

  uint32_t a_identity;
  uint32_t a_hyper;
  int x;
  const moria_vertex *A;
  const moria_vertex *B;

  a_identity = a->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(a_identity, b->attrs & MORIA_EDGE_IDENTITY))) {
    return r;
  }

  a_hyper = a->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(a_hyper, b->attrs & MORIA_EDGE_HYPER))) {
    return r;
  }

  if(a_identity)
  {
    A = a->A;
    B = b->A;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len))) {
      return r;
    }
  }
  else if(!a_hyper)
  {
    if((r = INTCMP(a->A, b->A))) {
      return r;
    }
  }
  else
  {
    if((r = INTCMP(a->Alen, b->Alen))) {
      return r;
    }

    for(x = 0; x < a->Alen; x++)
    {
      if((r = INTCMP(a->Alist[x].v, b->Alist[x].v))) {
        return r;
      }
    }

    if((r = INTCMP(a->Blen, b->Blen))) {
      return r;
    }

    for(x = 0; x < a->Blen; x++)
    {
      if((r = INTCMP(a->Blist[x].v, b->Blist[x].v))) {
        return r;
      }
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
  const moria_edge * b;
  const moria_vertex *A;
  const moria_vertex *B;

  b = containerof(bn, moria_edge, rbn_up);

  identity = key->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(identity, b->attrs & MORIA_EDGE_IDENTITY))) {
    return r;
  }

  hyper = key->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(hyper, b->attrs & MORIA_EDGE_HYPER))) {
    return r;
  }

  if(identity)
  {
    A = key->Alist[0];
    B = b->A;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len))) {
      return r;
    }
  }
  else if(!hyper)
  {
    if((r = INTCMP(key->Alist[0], b->A))) {
      return r;
    }
  }
  else
  {
    if((r = INTCMP(key->Alen, b->Alen))) {
      return r;
    }

    for(x = 0; x < key->Alen; x++)
    {
      if((r = INTCMP(key->Alist[x], b->Alist[x].v))) {
        return r;
      }
    }

    if((r = INTCMP(key->Blen, b->Blen))) {
      return r;
    }

    for(x = 0; x < key->Blen; x++)
    {
      if((r = INTCMP(key->Blist[x], b->Blist[x].v))) {
        return r;
      }
    }
  }

  return 0;
}

int edge_cmp_rbn_up(const rbnode * restrict an, const rbnode * restrict bn)
{
  const moria_edge * restrict a;
  const moria_edge * restrict b;

  if((a = an->ud.p) == 0) {
    a = containerof(an, moria_edge, rbn_up);
  }

  if((b = bn->ud.p) == 0) {
    b = containerof(bn, moria_edge, rbn_up);
  }

  return edge_cmp_up(a, b);
}


int edge_cmp_down(const moria_edge * restrict a, const moria_edge * restrict b)
{
  int r;

  uint32_t a_identity;
  uint32_t a_hyper;
  int x;
  const moria_vertex *A;
  const moria_vertex *B;

  a_identity = a->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(a_identity, b->attrs & MORIA_EDGE_IDENTITY))) {
    return r;
  }

  a_hyper = a->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(a_hyper, b->attrs & MORIA_EDGE_HYPER))) {
    return r;
  }

  if(a_identity)
  {
    A = a->B;
    B = b->B;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len))) {
      return r;
    }
  }
  else if(!a_hyper)
  {
    if((r = INTCMP(a->B, b->B))) {
      return r;
    }
  }
  else
  {
    if((r = INTCMP(a->Alen, b->Alen))) {
      return r;
    }

    for(x = 0; x < a->Alen; x++)
    {
      if((r = INTCMP(a->Alist[x].v, b->Alist[x].v))) {
        return r;
      }
    }

    if((r = INTCMP(a->Blen, b->Blen))) {
      return r;
    }

    for(x = 0; x < a->Blen; x++)
    {
      if((r = INTCMP(a->Blist[x].v, b->Blist[x].v))) {
        return r;
      }
    }
  }

  return 0;
}

int edge_cmp_key_down(void * _key, const rbnode * restrict bn)
{
  int r;

  uint32_t identity;
  uint32_t hyper;
  const edge_key * key = _key;
  const moria_edge * b;
  const moria_vertex *A;
  const moria_vertex *B;
  int x;

  if((b = bn->ud.p) == 0) {
    b = containerof(bn, moria_edge, rbn_down);
  }

  identity = key->attrs & MORIA_EDGE_IDENTITY;
  if((r = INTCMP(identity, b->attrs & MORIA_EDGE_IDENTITY))) {
    return r;
  }

  hyper = key->attrs & MORIA_EDGE_HYPER;
  if((r = INTCMP(hyper, b->attrs & MORIA_EDGE_HYPER))) {
    return r;
  }

  if(identity)
  {
    A = key->Blist[0];
    B = b->B;
    if((r = memncmp(A->label, A->label_len, B->label, B->label_len))) {
      return r;
    }
  }
  else if(!hyper)
  {
    if((r = INTCMP(key->Blist[0], b->B))) {
      return r;
    }
  }
  else
  {
    if((r = INTCMP(key->Alen, b->Alen))) {
      return r;
    }

    for(x = 0; x < key->Alen; x++)
    {
      if((r = INTCMP(key->Alist[x], b->Alist[x].v))) {
        return r;
      }
    }

    if((r = INTCMP(key->Blen, b->Blen))) {
      return r;
    }

    for(x = 0; x < key->Blen; x++)
    {
      if((r = INTCMP(key->Blist[x], b->Blist[x].v))) {
        return r;
      }
    }
  }

  return 0;
}

int edge_cmp_rbn_down(const rbnode * restrict an, const rbnode * restrict bn)
{
  const moria_edge * restrict a;
  const moria_edge * restrict b;

  if((a = an->ud.p) == 0) {
    a = containerof(an, moria_edge, rbn_down);
  }

  if((b = bn->ud.p) == 0) {
    b = containerof(bn, moria_edge, rbn_down);
  }

  return edge_cmp_down(a, b);
}

//
// api
//

void API moria_edge_init(struct moria_edge * const restrict e, struct moria_graph * const restrict g)
{
  graph_edge_init(g, e);
}

moria_edge * API moria_edge_between(const moria_vertex * restrict Ax, const moria_vertex * restrict Bx)
{
  moria_vertex * A = (void*)Ax;
  moria_vertex * B = (void*)Bx;

  rbnode * rbn;
  edge_key key;

  key = (typeof(key)) {
      attrs : MORIA_EDGE_IDENTITY
    , Blist : (moria_vertex*[]) { B }
    , Blen : 1
  };

  if((rbn = rbtree_lookup_node(&A->down, &key, edge_cmp_key_down)))
    return containerof(rbn, moria_edge, rbn_down);

  key.attrs &= ~MORIA_EDGE_IDENTITY;
  if((rbn = rbtree_lookup_node(&A->down, &key, edge_cmp_key_down)))
    return containerof(rbn, moria_edge, rbn_down);

  return 0;
}

moria_edge * API moria_edge_of(
    moria_vertex ** restrict Axlist
  , uint16_t Axlen
  , moria_vertex ** restrict Bxlist
  , uint16_t Bxlen
)
{
  edge_key key;
  rbnode *rbn;

  if(Axlen == 1 && Bxlen == 1) {
    return moria_edge_between(Axlist[0], Bxlist[0]);
  }

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
