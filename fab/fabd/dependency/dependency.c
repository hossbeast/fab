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

#include "xlinux/xstdlib.h"

#include "moria/edge.h"
#include "moria/operations.h"
#include "narrator.h"

#include "dependency.h"
#include "fsent.h"
#include "node_operations.h"
#include "formula.h"

llist dependency_list = LLIST_INITIALIZER(dependency_list);                 // active
static llist dependency_freelist = LLIST_INITIALIZER(dependency_freelist);  // free

//
// public
//

void dependency_release(dependency * restrict dep)
{
  if(dep->edge.attrs & MORIA_EDGE_HYPER)
  {
    wfree(dep->edge.Alist);
    wfree(dep->edge.Blist);
  }

  llist_delete(dep, edge.owner);
  llist_append(&dependency_freelist, dep, edge.owner);
}

xapi dependency_alloc(dependency ** restrict rv, moria_graph * restrict g, uint16_t Alen, uint16_t Blen)
{
  enter;

  dependency *dep;

  if((dep = llist_shift(&dependency_freelist, typeof(*dep), edge.owner)) == 0)
  {
    fatal(xmalloc, &dep, sizeof(*dep));
  }

  if(Alen != 1 || Blen != 1)
  {
    fatal(xmalloc, &dep->edge.Alist, sizeof(*dep->edge.Alist) * Alen);
    fatal(xmalloc, &dep->edge.Blist, sizeof(*dep->edge.Blist) * Blen);
  }

  moria_edge_init(&dep->edge, g);

  llist_append(&dependency_list, dep, edge.owner);
  *rv = dep;

  finally : coda;
}

xapi dependency_cleanup()
{
  enter;

  llist *T;
  dependency *dep;

  llist_foreach_safe(&dependency_list, dep, edge.owner, T) {
    dependency_release(dep);
  }

  llist_foreach_safe(&dependency_freelist, dep, edge.owner, T) {
    wfree(dep);
  }

  finally : coda;
}

void dependency_sources(const dependency * restrict bpe, llist * restrict list)
{
  moria_vertex *v;
  fsent *n;
  const moria_edge *e;
  int x;

  e = &bpe->edge;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    n = containerof(e->B, fsent, vertex);
    v = &n->vertex;
    llist_append(list, v, lln);
  }
  else
  {
    for(x = 0; x < e->Blen; x++)
    {
      n = containerof(e->Blist[x].v, fsent, vertex);
      v = &n->vertex;
      llist_append(list, v, lln);
    }
  }
}

void dependency_targets(const dependency * restrict bpe, llist * restrict list)
{
  moria_vertex *v;
  fsent *n;
  const moria_edge *e;
  int x;

  e = &bpe->edge;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    n = containerof(e->A, fsent, vertex);
    v = &n->vertex;
    llist_append(list, v, lln);
  }
  else
  {
    for(x = 0; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      v = &n->vertex;
      llist_append(list, v, lln);
    }
  }
}

xapi dependency_say_sources(const dependency * restrict bpe, narrator * restrict N)
{
  enter;

  llist list = LLIST_INITIALIZER(list);
  moria_vertex *v;
  fsent *n;
  int x;

  dependency_sources(bpe, &list);
  x = 0;
  llist_foreach(&list, v, lln) {
    n = containerof(v, fsent, vertex);
    if(x++) {
      fatal(narrator_xsays, N, ", ");
    }
    fatal(fsent_path_say, n, N);
  }

  finally : coda;
}

xapi dependency_say_targets(const dependency * restrict bpe, narrator * restrict N)
{
  enter;

  llist list = LLIST_INITIALIZER(list);
  moria_vertex *v;
  fsent *n;
  int x;

  dependency_targets(bpe, &list);
  x = 0;
  llist_foreach(&list, v, lln) {
    n = containerof(v, fsent, vertex);
    if(x++) {
      fatal(narrator_xsays, N, ", ");
    }
    fatal(fsent_path_say, n, N);
  }

  finally : coda;
}

xapi dependency_connect(
    fsent * restrict A
  , fsent * restrict B
  , edge_kind relation
  , graph_invalidation_context * restrict invalidation
  , dependency ** restrict nep
)
{
  enter;

  RUNTIME_ASSERT(relation == EDGE_DEPENDS || relation == EDGE_CONDUIT);

//  bool created = false;
  moria_edge *e;
  dependency *ne;
  moria_connect_context ctx;
  int r;

  r = moria_preconnect(&ctx, &g_graph, &A->vertex, &B->vertex, relation, &e);
  if(r == MORIA_HASEDGE)
  {
    *nep = containerof(e, dependency, edge);
    goto XAPI_FINALLY;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  fatal(dependency_alloc, &ne, &g_graph, 1, 1);
//  moria_connect(&ctx, &g_graph, &ne->edge, &A->vertex, &B->vertex, relation);
  fatal(graph_connect, &ctx, &A->vertex, &B->vertex, &ne->edge, relation);
  *nep = ne;

#if 0
  if(oldp) {
    oldvp = &old_vertex;
    *oldp = 0;
  }

  fatal(moria_connect_replace
    , g_graph
    , &A->vertex
    , &B->vertex
    , relation
    , &e
    , &created
    , oldvp
  );

  ne = edge_value(e);
  if(nep)
    *nep = ne;

  if(!created)
    goto XAPI_FINALLY;

  if(oldp && old_vertex) {
    *oldp = vertex_value(old_vertex);
  }
#endif

  llist_init_node(&ne->dependencies_lln);

  /* invalidate the target */
  fatal(fsent_invalidate, A, invalidation);

  /* vivify the source */
  if(fsent_state_get(B) == VERTEX_UNLINKED) {
    fsent_state_set(B, VERTEX_UNCREATED);
  }

  finally : coda;
}

xapi dependency_hyperconnect(
    moria_vertex ** restrict Alist
  , uint16_t Alen
  , moria_vertex ** restrict Blist
  , uint16_t Blen
  , edge_kind relation
  , graph_invalidation_context * restrict invalidation
  , dependency ** restrict nep
)
{
  enter;

  RUNTIME_ASSERT(relation == EDGE_DEPENDS || relation == EDGE_CONDUIT);

  int x;
  int r;
  moria_edge *e;
  dependency *ne;
  moria_connect_context ctx;
  fsent *B;

  r = moria_preconnect_hyper(&ctx, &g_graph, Alist, Alen, Blist, Blen, relation | MORIA_EDGE_HYPER, &e);
  if(r == MORIA_HASEDGE)
  {
    *nep = containerof(e, dependency, edge);
    goto XAPI_FINALLY;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  fatal(dependency_alloc, &ne, &g_graph, Alen, Blen);
  fatal(graph_hyperconnect, &ctx, Alist, Alen, Blist, Blen, &ne->edge, relation | MORIA_EDGE_HYPER);
//  moria_connect_hyper(&ctx, &g_graph, &ne->edge, Alist, Alen, Blist, Blen, relation | MORIA_EDGE_HYPER);
  *nep = ne;

#if 0
  created = false;
  e = 0;
  fatal(moria_hyperconnect
    , g_graph
    , Alist
    , Alen
    , Blist
    , Blen
    , relation
    , &e
    , &created
  );

  ne = edge_value(e);
  if(nep)
    *nep = ne;

  if(!created)
    goto XAPI_FINALLY;
#endif

  llist_init_node(&ne->dependencies_lln);

  /* when the edge is created, invalidate the targets */
  for(x = 0; x < Alen; x++)
  {
    fatal(fsent_invalidate, containerof(Alist[x], fsent, vertex), invalidation);
  }

  /* vivify the sources */
  for(x = 0; x < Blen; x++)
  {
    B = containerof(Blist[x], fsent, vertex);
    if(fsent_state_get(B) == VERTEX_UNLINKED) {
      fsent_state_set(B, VERTEX_UNCREATED);
    }
  }

  finally : coda;
}

xapi dependency_disconnect(dependency * restrict dep, graph_invalidation_context * restrict invalidation)
{
  enter;

  fsent *n;
  int x;

/*

1. unset the bpe for connected nodes
2. invalidate consumer nodes
3. formula reference counting

*/

  if(!(dep->edge.attrs & MORIA_EDGE_HYPER))
  {
    n = containerof(dep->edge.B, fsent, vertex);
    if(n->dep == dep) {
      n->dep = 0;
    }

    fatal(fsent_invalidate, n, invalidation);
  }
  else
  {
    for(x = 0; x < dep->edge.Blen; x++)
    {
      n = containerof(dep->edge.Blist[x].v, fsent, vertex);
      if(n->dep == dep) {
        n->dep = 0;
      }

      fatal(fsent_invalidate, n, invalidation);
    }
  }

  if(dep->fml) {
    RUNTIME_ASSERT(--dep->fml->self_fml->refs >= 0);
//printf("FML %p refs %d -> %d\n", dep->fml->self_fml, dep->fml->self_fml->refs + 1, dep->fml->self_fml->refs);
  }


  dep->rme = 0;
  dep->fml = 0;

  fatal(graph_disconnect, &dep->edge);
  dependency_release(dep);

  finally : coda;
}


#if 0
  e = &dep->edge;
  if(!(e->attrs & MORIA_EDGE_HYPER)) {
    n = containerof(e->A, fsent, vertex);
    if(n->dep == dep) {
      n->dep = 0;
    }
  } else {
    for(x = 0; x < e->Alen; x++) {
      n = containerof(e->Alist[x].v, fsent, vertex);
      if(n->dep == dep) {
        n->dep = 0;
      }
    }
  }
#endif
