/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/connect.h"
#include "moria/graph.h"
#include "moria/edge.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "ref_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "filesystem.h"
#include "edge.h"

llist ref_edge_list = LLIST_INITIALIZER(ref_edge_list);   // active
static llist freelist = LLIST_INITIALIZER(freelist);      // free

static void ref_edge_dispose(ref_edge * restrict se)
{
  llist_delete(&se->edge.owner);
  llist_append(&freelist, &se->edge.owner);
  ASAN_TAINT(se);
}

ref_edge *ref_edge_alloc()
{
  ref_edge *se;

  if((se = llist_shift(&freelist, typeof(*se), edge.owner))) {
    ASAN_CLEAN(se);
    memset(se, 0, sizeof(*se));
  } else {
    xmalloc(&se, sizeof(*se));
  }

  moria_edge_init(&se->edge, &g_graph);
  llist_append(&ref_edge_list, &se->edge.owner);

  return se;
}

void ref_edge_release(ref_edge * restrict se)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(moria_edge_degree(&se->edge) == 0);

  ref_edge_dispose(se);
}

void ref_edge_setup(void)
{
}

void ref_edge_cleanup(void)
{
  ref_edge *se;

  while((se = llist_shift(&freelist, typeof(*se), edge.owner))) {
    ASAN_CLEAN(se);
    free(se);
  }
}

ref_edge *ref_connect(
    struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , edge_type type
)
{
  moria_edge *e;
  ref_edge *nse;
  moria_connect_context ctx;
  int r;

  r = moria_preconnect(&ctx, &g_graph, B, A, type, &e);
  if(r == MORIA_HASEDGE) {
    nse = containerof(e, typeof(*nse), edge);
    return nse;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  nse = ref_edge_alloc();
  moria_connect(&ctx, &g_graph, &nse->edge, B, A, type);

  return nse;
}

void ref_disconnecting(ref_edge * restrict ref, graph_invalidation_context * restrict invalidation)
{
}

#if CONFIG_DEVEL
void ref_edges_show()
{
  ref_edge *e;
  int x;

  printf("ref_edges, live : %zu free : %zu\n", llist_count(&ref_edge_list), llist_count(&freelist));
  x = 0;
  llist_foreach(&ref_edge_list, e, edge.owner) {
    printf("%3u ", x++);
    edge_identity_show(&e->edge);
  }
}
#endif
