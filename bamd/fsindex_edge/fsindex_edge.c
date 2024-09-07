/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/graph.h"
#include "moria/edge.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "fsindex_edge.h"
#include "fsent.h"
#include "vertex.h"
#include "stats.h"
#include "filesystem.h"

llist index_edge_list = LLIST_INITIALIZER(index_edge_list);                 // active
static llist index_edge_freelist = LLIST_INITIALIZER(index_edge_freelist);  // free

static void fsindex_edge_dispose(fsindex_edge * restrict se)
{
  llist_delete(&se->edge.owner);
  llist_append(&index_edge_freelist, &se->edge.owner);
}

fsindex_edge *fsindex_edge_alloc()
{
  fsindex_edge *se;

  if((se = llist_shift(&index_edge_freelist, typeof(*se), edge.owner)) == 0) {
    xmalloc(&se, sizeof(*se));
  } else {
    memset(se, 0, sizeof(*se));
  }

  moria_edge_init(&se->edge, &g_graph);
  llist_append(&index_edge_list, &se->edge.owner);

  return se;
}

void fsindex_edge_release(fsindex_edge * restrict se)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(!rbnode_attached(&se->edge.rbn_up));
  RUNTIME_ASSERT(!rbnode_attached(&se->edge.rbn_down));

  fsindex_edge_dispose(se);
}

void fsindex_edge_setup(void)
{
}

void fsindex_edge_cleanup(void)
{
  fsindex_edge *se;
  llist *T;

  llist_foreach_safe(&index_edge_list, se, edge.owner, T) {
    fsindex_edge_dispose(se);
  }

  llist_foreach_safe(&index_edge_freelist, se, edge.owner, T) {
    free(se);
  }
}
