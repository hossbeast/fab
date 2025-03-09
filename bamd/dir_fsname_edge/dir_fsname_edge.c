/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/connect.h"
#include "moria/graph.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "dir_fsname_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "filesystem.h"
#include "edge.h"
#include "fsname.h"
#include "namespace.h"
#include "reconcile.h"
#include "edges.h"
#include "vertices.h"
#include "disintegrate.h"

llist dir_fsname_edge_list = LLIST_INITIALIZER(dir_fsname_edge_list);                 // active
static llist dir_fsname_edge_freelist = LLIST_INITIALIZER(dir_fsname_edge_freelist);  // free

static void dir_fsname_edge_dispose(dir_fsname_edge * restrict se)
{
  llist_delete(&se->edge.owner);
  llist_append(&dir_fsname_edge_freelist, &se->edge.owner);
}

dir_fsname_edge *dir_fsname_edge_alloc()
{
  dir_fsname_edge *se;

  if((se = llist_shift(&dir_fsname_edge_freelist, typeof(*se), edge.owner)) == 0) {
    xmalloc(&se, sizeof(*se));
  } else {
    memset(se, 0, sizeof(*se));
  }

  moria_edge_init(&se->edge, &g_graph);
  llist_append(&dir_fsname_edge_list, &se->edge.owner);

  return se;
}

void dir_fsname_edge_release(dir_fsname_edge * restrict se)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(moria_edge_degree(&se->edge) == 0);

  dir_fsname_edge_dispose(se);
}

void dir_fsname_edge_setup(void)
{
}

void dir_fsname_edge_cleanup(void)
{
  dir_fsname_edge *e;

  while((e = llist_shift(&dir_fsname_edge_freelist, typeof(*e), edge.owner))) {
    free(e);
  }
}

void dir_fsname_edge_disconnecting(dir_fsname_edge * restrict nse, graph_invalidation_context * restrict invalidation)
{
  fsname * restrict fsn;
  namespace * restrict ns = 0;
  moria_foreach_state st;
  moria_edge *edge;

  fsn = containerof(nse->edge.A, typeof(*fsn), vertex);
  moria_foreach_edge_up(&st, &fsn->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_FSNAME_NS:
        ns = containerof(edge->A, typeof(*ns), vertex);
      default:
        break;
    }
  }

  if(!ns) {
    return;
  }

  ns->namespace_stats.del++;
  fsn->fsname_stats.del++;
  if(--fsn->fsname_stats.count == 0) {
    ns->namespace_stats.count--;
  }
}

void dir_fsname_edge_reconcile(struct graph_invalidation_context * restrict invalidation)
{
  dir_fsname_edge *e;
  llist *T;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;

  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);
  llist_foreach_safe(&dir_fsname_edge_list, e, edge.owner, T) {
    if(e->epoch == reconciliation_epoch) {
      continue;
    }

    edges_push(&edges, &e->edge);
  }

  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
}

#if CONFIG_DEVEL
void dir_fsname_edges_show()
{
  dir_fsname_edge *e;
  int x;

  printf("dir_fsname_edges, live : %zu free : %zu\n", llist_count(&dir_fsname_edge_list), llist_count(&dir_fsname_edge_freelist));
  x = 0;
  llist_foreach(&dir_fsname_edge_list, e, edge.owner) {
    printf("%3u ", x++);
    edge_identity_show(&e->edge);
  }
}
#endif
