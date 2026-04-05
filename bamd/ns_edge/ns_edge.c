/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/connect.h"
#include "moria/graph.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "ns_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "filesystem.h"
#include "edge.h"
#include "fsname.h"
#include "namespace.h"

llist ns_edge_list = LLIST_INITIALIZER(ns_edge_list);   // active
static llist freelist = LLIST_INITIALIZER(freelist);    // free

static void ns_edge_dispose(ns_edge * restrict se)
{
  llist_delete(&se->edge.owner);
  llist_append(&freelist, &se->edge.owner);
  ASAN_TAINT(se);
}

ns_edge *ns_edge_alloc()
{
  ns_edge *se;

  if((se = llist_shift(&freelist, typeof(*se), edge.owner))) {
    ASAN_CLEAN(se);
    memset(se, 0, sizeof(*se));
  } else {
    xmalloc(&se, sizeof(*se));
  }

  moria_edge_init(&se->edge, &g_graph);
  llist_append(&ns_edge_list, &se->edge.owner);

  return se;
}

void ns_edge_release(ns_edge * restrict se)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(moria_edge_degree(&se->edge) == 0);

  ns_edge_dispose(se);
}

void ns_edge_setup(void)
{
}

void ns_edge_cleanup(void)
{
  ns_edge *e;

  while((e = llist_shift(&freelist, typeof(*e), edge.owner))) {
    ASAN_CLEAN(e);
    free(e);
  }
}

ns_edge *ns_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , edge_type type
)
{
  moria_edge *e;
  ns_edge *nse;
  moria_connect_context ctx;
  int r;

  r = moria_preconnect(&ctx, &g_graph, B, A, type, &e);
  if(r == MORIA_HASEDGE) {
    nse = containerof(e, typeof(*nse), edge);
    return nse;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  nse = ns_edge_alloc();
  moria_connect(&ctx, &g_graph, &nse->edge, B, A, type);

  return nse;
}


void ns_edge_disconnecting(ns_edge * restrict nse, graph_invalidation_context * restrict invalidation)
{
}

#if CONFIG_DEVEL
void ns_edges_show()
{
  ns_edge *e;
  int x;

  printf("ns_edges, live : %zu free : %zu\n", llist_count(&ns_edge_list), llist_count(&freelist));
  x = 0;
  llist_foreach(&ns_edge_list, e, edge.owner) {
    printf("%3u ", x++);
    edge_identity_show(&e->edge);
  }
}
#endif
