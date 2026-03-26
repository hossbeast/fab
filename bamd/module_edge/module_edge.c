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
#include "edges.h"
#include "vertices.h"

llist module_edge_list = LLIST_INITIALIZER(module_edge_list);   // active list
static llist freelist = LLIST_INITIALIZER(freelist);            // freelist

//
// static
//

static void module_edge_alloc(module_edge ** restrict mep)
{
  module_edge *me;

  if((me = llist_shift(&freelist, typeof(*me), edge.owner))) {
    ASAN_CLEAN(me);
    memset(me, 0, sizeof(*me));
  } else {
    xmalloc(&me, sizeof(*me));
  }

  moria_edge_init(&me->edge, &g_graph);

  llist_append(&module_edge_list, &me->edge.owner);
  *mep = me;
}

static void module_edge_dispose(module_edge * restrict me)
{
  //RUNTIME_ASSERT(me->edge.attrs & EDGE_MODULES);

  llist_delete(&me->edge.owner);
  llist_append(&freelist, &me->edge.owner);
  ASAN_TAINT(me);
}

void module_edge_release(module_edge * restrict me)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(moria_edge_degree(&me->edge) == 0);

  module_edge_dispose(me);
}

bool module_edge_connect(moria_vertex * restrict B, module * restrict A, edge_type type, module_edge ** restrict mmep)
{
  module_edge *mme;
  moria_edge *e;
  moria_connect_context ctx;
  int r;
  bool b;

  switch(type) {
    case EDGE_RULESET_BUILD_MOD:
    case EDGE_RULESET_ANCHOR_MOD:
    case EDGE_RULESET_USES_MOD:
      RUNTIME_ASSERT(vertex_type_get(B) == VERTEX_RULESET);
      RUNTIME_ASSERT(vertex_type_get(&A->vertex) == VERTEX_MODULE);
      break;
    case EDGE_VARSET_MOD:
      RUNTIME_ASSERT(vertex_type_get(B) == VERTEX_VARSET);
      RUNTIME_ASSERT(vertex_type_get(&A->vertex) == VERTEX_MODULE);
      break;
    default:
      break;
  }

  r = moria_preconnect(&ctx, &g_graph, B, &A->vertex, type, &e);
  if(r == MORIA_HASEDGE) {
    mme = containerof(e, typeof(*mme), edge);
    if(mmep) {
      *mmep = mme;
    }
    b = false;

    goto end;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  module_edge_alloc(&mme);
  moria_connect(&ctx, &g_graph, &mme->edge, B, &A->vertex, type);
  if(mmep) {
    *mmep = mme;
  }

  b = true;

end:
  mme->epoch = reconciliation_epoch;
  return b;
}

void module_edge_disconnecting(
    module_edge * restrict mme
  , graph_invalidation_context * restrict invalidation
)
{
  /* invalidate the module */
  switch(edge_type_get(&mme->edge)) {
    case EDGE_RULESET_BUILD_MOD:
    case EDGE_RULESET_ANCHOR_MOD:
    case EDGE_RULESET_USES_MOD:
    case EDGE_VARSET_MOD:
      vertex_invalidate(mme->edge.A, invalidation);
      break;
    default:
      RTA;
  }
}

void module_module_cleanup()
{
  module_edge *me;

  while((me = llist_shift(&freelist, typeof(*me), edge.owner))) {
    ASAN_CLEAN(me);
    free(me);
  }
}

#if CONFIG_DEVEL
void module_edges_show()
{
  module_edge *e;
  int x;

  printf("module_edges, live : %zu free : %zu\n", llist_count(&module_edge_list), llist_count(&freelist));
  x = 0;
  llist_foreach(&module_edge_list, e, edge.owner) {
    printf("%3u ", x++);
    edge_identity_show(&e->edge);
  }
}
#endif
