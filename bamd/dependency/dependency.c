/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "moria/edge.h"
#include "moria/foreach.h"
#include "moria/connect.h"
#include "narrator.h"

#include "dependency.h"
#include "fsent.h"
#include "vertex.h"
#include "formula.h"
#include "bpe.h"
#include "edge.h"
#include "edges.h"
#include "vertices.h"

llist dependency_list = LLIST_INITIALIZER(dependency_list);                 // active
static llist dependency_freelist = LLIST_INITIALIZER(dependency_freelist);  // free

static void dependency_destroy(dependency * restrict dep)
{
  if(dep->edge.attrs & MORIA_EDGE_HYPER)
  {
    free(dep->edge.Alist);
    free(dep->edge.Blist);
  }
}

static void dependency_dispose(dependency * restrict dep)
{
  dependency_destroy(dep);

  llist_delete(&dep->edge.owner);
  llist_append(&dependency_freelist, &dep->edge.owner);
}

void dependency_release(dependency * restrict dep)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(moria_edge_degree(&dep->edge) == 0);

  dependency_dispose(dep);
}

//
// public
//

static void dependency_alloc(dependency ** restrict rv, moria_graph * restrict g, uint16_t Alen, uint16_t Blen)
{
  dependency *dep;

  if((dep = llist_shift(&dependency_freelist, typeof(*dep), edge.owner)) == 0)
  {
    xmalloc(&dep, sizeof(*dep));
  }

  if(Alen != 1 || Blen != 1)
  {
    xmalloc(&dep->edge.Alist, sizeof(*dep->edge.Alist) * Alen);
    xmalloc(&dep->edge.Blist, sizeof(*dep->edge.Blist) * Blen);
  }

  moria_edge_init(&dep->edge, g);

  llist_append(&dependency_list, &dep->edge.owner);
  *rv = dep;
}

void dependency_cleanup()
{
  dependency *dep;

  while((dep = llist_shift(&dependency_freelist, typeof(*dep), edge.owner))) {
    free(dep);
  }
}

void dependency_connect(moria_vertex * restrict B, moria_vertex * restrict A, edge_type relation)
{
  moria_edge *e;
  dependency *dep;
  moria_connect_context ctx;
  int r;

  r = moria_preconnect(&ctx, &g_graph, B, A, relation, &e);
  if(r == MORIA_HASEDGE) {
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  dependency_alloc(&dep, &g_graph, 1, 1);
  moria_connect(&ctx, &g_graph, &dep->edge, B, A, relation);

  /* update cached fields */
  switch(relation) {
    case EDGE_SRC_BPE:
      RUNTIME_ASSERT(vertex_kind_get(B) == VERTEX_KIND_FSENT);
      RUNTIME_ASSERT(vertex_type_get(A) == VERTEX_BPE);
      break;
    case EDGE_BPE_TGT:
      RUNTIME_ASSERT(vertex_type_get(B) == VERTEX_BPE);
      RUNTIME_ASSERT(vertex_kind_get(A) == VERTEX_KIND_FSENT);
      dep->bpe = containerof(B, buildplan_entity, vertex);
      break;
    default:
      RTA;
  }

  if(relation == EDGE_BPE_TGT) {
  }
}

void dependency_hyperconnect(
    moria_vertex ** restrict Blist
  , uint16_t Blen
  , moria_vertex ** restrict Alist
  , uint16_t Alen
  , edge_type graph
)
{
  int r;
  moria_edge *e;
  dependency *ne;
  moria_connect_context ctx;

  r = moria_preconnect_hyper(&ctx, &g_graph, Blist, Blen, Alist, Alen, graph | MORIA_EDGE_HYPER, &e);
  if(r == MORIA_HASEDGE) {
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  dependency_alloc(&ne, &g_graph, Alen, Blen);

  moria_connect_hyper(&ctx, &g_graph, &ne->edge, Blist, Blen, Alist, Alen, graph | MORIA_EDGE_HYPER);

  /* updated cached fields */
  if(graph == EDGE_BPE_TGT) {
    ne->bpe = containerof(Blist[0], buildplan_entity, vertex);
  }
}

void dependency_disconnecting(
    dependency * restrict dep
  , graph_invalidation_context * restrict invalidation
)
{
  moria_vertex *v;
  fsent *A;
  struct vertices list, *eln;
  int x;
  moria_edge *e;
  moria_foreach_state st;

  if(edge_type_get(&dep->edge) != EDGE_BPE_TGT) {
    return;
  }

  vertices_init(&list);
  if(!(dep->edge.attrs & MORIA_EDGE_HYPER)) {
    vertices_push(&list, dep->edge.A);
  } else {
    for(x = 0; x < dep->edge.Alen; x++) {
      vertices_push(&list, dep->edge.Alist[x].v);
    }
  }

  /* update cached fields */
  vertices_foreach(&list, v, eln) {
    A = containerof(v, typeof(*A), vertex);
    moria_foreach_edge_down(&st, &A->vertex, e) {
      if(edge_type_get(e) != EDGE_BPE_TGT) {
        continue;
      }
      if(e != &dep->edge) {
        break;
      }
    }
    if(st.next == 0) {
      if(A->bpe != dep->bpe) {
        continue;
      }
      if(A->bpe != 0) {
        fsent_unlink_candidate(A);
      }
      A->bpe = 0;
    }
  }

  vertices_destroy(&list);
}
