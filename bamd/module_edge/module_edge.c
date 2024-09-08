/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/edge.h"
#include "moria/vertex.h"
#include "moria/connect.h"
#include "valyria/llist.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "module_edge.h"
#include "fsent.h"
#include "fsedge.h"
#include "vertex.h"
#include "edge.h"
#include "reconcile.h"
#include "entlist.h"

llist module_edge_list = LLIST_INITIALIZER(module_edge_list);          // active list
static llist module_edge_freelist = LLIST_INITIALIZER(module_edge_freelist);  // freelist

//
// static
//

static void module_edge_alloc(module_edge ** restrict mep)
{
  module_edge *me;

  if((me = llist_shift(&module_edge_freelist, typeof(*me), edge.owner)) == 0) {
    xmalloc(&me, sizeof(*me));
  }

  moria_edge_init(&me->edge, &g_graph);

  llist_append(&module_edge_list, &me->edge.owner);
  *mep = me;
}

static void module_edge_dispose(module_edge * restrict me)
{
  RUNTIME_ASSERT(me->edge.attrs & EDGE_MODULES);

  llist_delete(&me->edge.owner);
  llist_append(&module_edge_freelist, &me->edge.owner);
}

void module_edge_release(module_edge * restrict me)
{
  int x;

  /* this edge must have been previously excised from the graph */
  if(me->edge.attrs & MORIA_EDGE_HYPER)
  {
    for(x = 0; x < me->edge.Alen; x++) {
      RUNTIME_ASSERT(!rbnode_attached(&me->edge.Alist[x].rbn));
    }
    for(x = 0; x < me->edge.Blen; x++) {
      RUNTIME_ASSERT(!rbnode_attached(&me->edge.Blist[x].rbn));
    }
  }
  else
  {
    RUNTIME_ASSERT(!rbnode_attached(&me->edge.rbn_up));
    RUNTIME_ASSERT(!rbnode_attached(&me->edge.rbn_down));
  }

  module_edge_dispose(me);
}

bool module_edge_connect(module * restrict A, moria_vertex * restrict B, edge_type type, module_edge ** restrict mmep)
{
  module_edge *mme;
  moria_edge *e;
  moria_connect_context ctx;
  int r;
  bool b;

  switch(type) {
    case EDGE_RULESET_MOD:
      RUNTIME_ASSERT(vertex_type_get(B) == VERTEX_RULESET);
      break;
    case EDGE_REQUIRES:
      RUNTIME_ASSERT(vertex_type_get(B) == VERTEX_MODULE);
      break;
    case EDGE_VARSET_MOD:
      RUNTIME_ASSERT(vertex_type_get(B) == VERTEX_VARSET);
      break;
    default:
      break;
  }

  r = moria_preconnect(&ctx, &g_graph, B, &A->vertex, type, &e);
  if(r == MORIA_HASEDGE) {
    mme = containerof(e, typeof(*mme), edge);
    *mmep = mme;
    b = false;

    goto end;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  module_edge_alloc(&mme);
  moria_connect(&ctx, &g_graph, &mme->edge, B, &A->vertex, type);
  *mmep = mme;

  b = true;

end:
  mme->reconciliation_epoch = reconciliation_epoch;
  return b;
}

void module_edge_disconnecting(
    module_edge * restrict mme
  , entlist * restrict edges
  , entlist * restrict vertices
  , graph_invalidation_context * restrict invalidation
)
{
  /* invalidate the module */
  switch(edge_type_get(&mme->edge)) {
    case EDGE_RULESET_MOD:
    case EDGE_VARSET_MOD:
      vertex_invalidate(mme->edge.A, invalidation);
    case EDGE_REQUIRES:
      break;
    default:
      RTA;
  }

  /* also removed related edges, e.g. //module/imports/foo */
  if(mme->related) {
    entlist_push(edges, &mme->related->edge);
  }
}

void module_module_cleanup()
{
  module_edge *me;
  llist *T;

  llist_foreach_safe(&module_edge_list, me, edge.owner, T) {
    module_edge_dispose(me);
  }

  llist_foreach_safe(&module_edge_freelist, me, edge.owner, T) {
    free(me);
  }
}
