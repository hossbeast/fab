/* COPYRIGHT (C) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include "operations.h"
#include "edge.internal.h"
#include "moria.h"

int API moria_preconnect(
    moria_connect_context * restrict ctx
  , moria_graph * restrict g
  , moria_vertex * restrict A
  , moria_vertex * B
  , uint32_t attrs
  , moria_edge ** restrict ep
)
{
  rbnode *rbn;
  moria_edge *e = 0;
  edge_key key;

  RUNTIME_ASSERT(!(attrs & MORIA_EDGE_HYPER));

  /* search by identity */
  key = (typeof(key)) {
      attrs : MORIA_EDGE_IDENTITY
    , Blist : &B
    , Blen : 1
  };
  if((rbn = rbtree_search(&A->down, &ctx->identity_search_context, &key, edge_cmp_key_down)))
  {
    e = containerof(rbn, moria_edge, rbn_down);
    if(ep) {
      *ep = e;
    }

    if(e->B == B && e->attrs == attrs) {
      /* this identity edge already exists */
      return MORIA_HASEDGE;
    }
    else if(e->B == B)
    {
      /* an identity edge to this vertex exists, but with different attrs */
      return MORIA_EDGEATTRS;
    }
    else if(attrs & MORIA_EDGE_IDENTITY)
    {
      /* an identity edge exists to another vertex with the same label */
      return MORIA_EDGEDOWN;
    }
  }

  /* search by vertex */
  key.attrs &= ~MORIA_EDGE_IDENTITY;
  if((rbn = rbtree_search(&A->down, &ctx->vertex_search_context, &key, edge_cmp_key_down)))
  {
    e = containerof(rbn, moria_edge, rbn_down);
    if(ep) {
      *ep = e;
    }

    if(e->attrs == attrs) {
      /* this non-identity edge already exists */
      return MORIA_HASEDGE;
    }

    /* this non-identity edge to this vertex exists, but with different attrs */
    return MORIA_EDGEATTRS;
  }

  /* check non-rbtree upwards identity edge */
  if((attrs & MORIA_EDGE_IDENTITY) && B->up_identity && B->up_identity->A != A) {
    /* upwards identity edge for B already exists, but with a different A vertex */
    return MORIA_EDGEUP;
  }

  return MORIA_NOEDGE;
}

void API moria_connect(
    moria_connect_context * restrict ctx
  , moria_graph * const restrict g
  , moria_edge * const restrict e
  , moria_vertex * const restrict A
  , moria_vertex * const restrict B
  , uint32_t attrs
)
{
  rbnode __attribute__((unused)) *r;

  RUNTIME_ASSERT(!(attrs & MORIA_EDGE_HYPER));

  e->attrs = attrs;
  e->A = A;
  e->B = B;

  /* insert into B->up */
  if(attrs & MORIA_EDGE_IDENTITY)
  {
    B->up_identity = e;
  }
  else
  {
    r = rbtree_put_node(&B->up, &e->rbn_up, edge_cmp_rbn_up);
    RUNTIME_ASSERT(r == 0);
  }

  /* insert into A->down, reusing the previous search results in A->down */
  if(attrs & MORIA_EDGE_IDENTITY)
  {
    rbtree_insert_node(&A->down, &ctx->identity_search_context, &e->rbn_down);
  }
  else
  {
    rbtree_insert_node(&A->down, &ctx->vertex_search_context, &e->rbn_down);
  }
}

int API moria_preconnect_hyper(
    moria_connect_context * restrict ctx
  , moria_graph * const restrict g
  , moria_vertex ** const restrict Alist
  , uint16_t Alen
  , moria_vertex ** const restrict Blist
  , uint16_t Blen
  , uint32_t attrs
  , moria_edge ** const restrict ep
)
{
  edge_key key;
  rbnode *rbn = 0;
  moria_edge *e;

  RUNTIME_ASSERT(!(attrs & MORIA_EDGE_IDENTITY));
  RUNTIME_ASSERT(attrs & MORIA_EDGE_HYPER);
  RUNTIME_ASSERT(Alen + Blen);

  if(Alen == 1 && Blen == 1) {
    return moria_preconnect(ctx, g, Alist[0], Blist[0], attrs & ~MORIA_EDGE_HYPER, ep);
  }

  if(Alen)
    qsort(Alist, Alen, sizeof(*Alist), ptrcmp);
  if(Blen)
    qsort(Blist, Blen, sizeof(*Blist), ptrcmp);

  // search by vertex-set (non-identity)
  key = (typeof(key)) {
      attrs : MORIA_EDGE_HYPER
    , Alist : Alist
    , Alen : Alen
    , Blist : Blist
    , Blen : Blen
  };
  if(Alen)
  {
    if((rbn = rbtree_search(&Alist[0]->down, &ctx->vertex_search_context, &key, edge_cmp_key_down))) {
      e = rbn->ud.p;
    }
  }
  else
  {
    if((rbn = rbtree_search(&Blist[0]->up, &ctx->vertex_search_context, &key, edge_cmp_key_up))) {
      e = rbn->ud.p;
    }
  }

  if(rbn)
  {
    if(ep) {
      *ep = e;
    }
    if(e->attrs == attrs) {
      return MORIA_HASEDGE;
    }

    /* a hyperedge with these vertices already exists, but with different attrs */
    return MORIA_EDGEATTRS;
  }

  return MORIA_NOEDGE;
}

void API moria_connect_hyper(
    moria_connect_context * restrict ctx
  , moria_graph * const restrict g
  , moria_edge * const restrict e
  , moria_vertex ** const restrict Alist
  , uint16_t Alen
  , moria_vertex ** const restrict Blist
  , uint16_t Blen
  , uint32_t attrs
)
{
  int x;

  RUNTIME_ASSERT(!(attrs & MORIA_EDGE_IDENTITY));
  RUNTIME_ASSERT(attrs & MORIA_EDGE_HYPER);
  RUNTIME_ASSERT(Alen + Blen);

  if(Alen == 1 && Blen == 1) {
    return moria_connect(ctx, g, e, Alist[0], Blist[0], attrs & ~MORIA_EDGE_HYPER);
  }

  e->attrs = attrs;

  for(x = 0; x < Alen; x++)
  {
    e->Alist[x].v = Alist[x];
    e->Alist[x].rbn.ud.p = e;
  }
  e->Alen = Alen;

  for(x = 0; x < Blen; x++)
  {
    e->Blist[x].v = Blist[x];
    e->Blist[x].rbn.ud.p = e;
  }
  e->Blen = Blen;

  for(x = 0; x < Alen; x++)
  {
    if(x == 0) {
      rbtree_insert_node(&Alist[x]->down, &ctx->vertex_search_context, &e->Alist[x].rbn);
    } else {
      rbtree_put_node(&Alist[x]->down, &e->Alist[x].rbn, edge_cmp_rbn_down);
    }
  }

  for(x = 0; x < Blen; x++)
  {
    if(x == 0 && !Alen) {
      rbtree_insert_node(&Blist[x]->up, &ctx->vertex_search_context, &e->Blist[x].rbn);
    } else {
      rbtree_put_node(&Blist[x]->up, &e->Blist[x].rbn, edge_cmp_rbn_up);
    }
  }
}

xapi API moria_edge_disconnect(
    moria_graph * const restrict g
  , moria_edge * const restrict e
)
{
  enter;

  int x;

  if((e->attrs & MORIA_EDGE_HYPER))
  {
    for(x = 0; x < e->Alen; x++)
    {
      rbtree_delete(&e->Alist[x].v->down, &e->Alist[x], rbn);
    }

    for(x = 0; x < e->Blen; x++)
    {
      rbtree_delete(&e->Blist[x].v->up, &e->Blist[x], rbn);
    }
  }
  else
  {
    rbtree_delete(&e->A->down, e, rbn_down);

    if(e->attrs & MORIA_EDGE_IDENTITY)
    {
      e->B->up_identity = 0;
    }
    else
    {
      rbtree_delete(&e->B->up, e, rbn_up);
    }
  }

  finally : coda;
}

xapi API moria_disconnect(
    moria_graph * const restrict g
  , moria_vertex * const restrict A
  , moria_vertex * const restrict B
)
{
  enter;

  moria_edge *e;

  if((e = moria_edge_between(A, B))) {
    fatal(moria_edge_disconnect, g, e);
  }

  finally : coda;
}

xapi API moria_hyperdisconnect(
    moria_graph * restrict g
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , moria_vertex ** restrict Blist
  , uint16_t Blen
)
{
  enter;

  moria_edge *e;

  if((e = moria_edge_of(Alist, Alen, Blist, Blen))) {
    fatal(moria_edge_disconnect, g, e);
  }

  finally : coda;
}
