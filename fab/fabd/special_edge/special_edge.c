/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/edge.h"
#include "moria/vertex.h"
#include "moria/connect.h"
#include "valyria/llist.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "special_edge.h"
#include "fsent.h"
#include "fsedge.h"
#include "vertex.h"
#include "reconcile.h"
#include "edge.h"
#include "entlist.h"
#include "formula.h"

static llist special_edge_list = LLIST_INITIALIZER(special_edge_list);          // active list
static llist special_edge_freelist = LLIST_INITIALIZER(special_edge_freelist);  // freelist

//
// static
//

static void special_edge_alloc(special_edge ** restrict mep)
{
  special_edge *se;

  if((se = llist_shift(&special_edge_freelist, typeof(*se), edge.owner)) == 0) {
    xmalloc(&se, sizeof(*se));
  } else {
    memset(se, 0, sizeof(*se));
  }

  moria_edge_init(&se->edge, &g_graph);

  llist_append(&special_edge_list, &se->edge.owner);
  *mep = se;
}

static void special_edge_dispose(special_edge * restrict se)
{
  llist_delete(&se->edge.owner);
  llist_append(&special_edge_freelist, &se->edge.owner);
}

void special_edge_release(special_edge * restrict se)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(!rbnode_attached(&se->edge.rbn_up));
  RUNTIME_ASSERT(!rbnode_attached(&se->edge.rbn_down));

  special_edge_dispose(se);
}

void special_connect(
    moria_vertex * restrict B
  , moria_vertex * restrict A
  , edge_type type
  , graph_invalidation_context * restrict invalidation
)
{
  special_edge *se;
  moria_edge *e;
  moria_connect_context ctx;
  int r;

  RUNTIME_ASSERT(vertex_kind_get(A) == VERTEX_KIND_SPECIAL);
  RUNTIME_ASSERT(vertex_special_get(A));
  RUNTIME_ASSERT(vertex_kind_get(B) == VERTEX_KIND_FSENT);
  RUNTIME_ASSERT(vertex_filetype_get(B) == VERTEX_FILETYPE_REG);

  r = moria_preconnect(&ctx, &g_graph, B, A, type, &e);
  if(r == MORIA_HASEDGE) {
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  special_edge_alloc(&se);
  moria_connect(&ctx, &g_graph, &se->edge, B, A, type);

  vertex_invalidate(se->edge.A, invalidation);
}

void special_edge_cleanup()
{
  special_edge *se;
  llist *T;

  llist_foreach_safe(&special_edge_list, se, edge.owner, T) {
    special_edge_dispose(se);
  }

  llist_foreach_safe(&special_edge_freelist, se, edge.owner, T) {
    free(se);
  }
}


void special_disconnecting(
    special_edge * restrict se
  , graph_invalidation_context * restrict invalidation
)
{
  vertex_invalidate(se->edge.A, invalidation);
  vertex_state_set(se->edge.A, VERTEX_UNLINKED);
}
