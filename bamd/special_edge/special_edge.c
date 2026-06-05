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
#include "edges.h"
#include "vertices.h"
#include "formula.h"
#include "config_system.h"
#include "namespace.h"
#include "varset.h"
#include "configset.h"
#include "ruleset.h"

llist special_edge_list = LLIST_INITIALIZER(special_edge_list);                 // active list
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
  RUNTIME_ASSERT(moria_edge_degree(&se->edge) == 0);

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
  moria_edge *ep;
  moria_connect_context ctx;
  module *mod;
  fsent *fse;
  int r;

  RUNTIME_ASSERT(vertex_kind_get(B) == VERTEX_KIND_FSENT);
  RUNTIME_ASSERT(type & EDGE_SPECIALS);

  r = moria_preconnect(&ctx, &g_graph, B, A, type, &ep);
  if(r == MORIA_HASEDGE) {
    se = containerof(ep, typeof(*se), edge);
    se->epoch = reconciliation_epoch;
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  special_edge_alloc(&se);
  moria_connect(&ctx, &g_graph, &se->edge, B, A, type);
  se->epoch = reconciliation_epoch;

  vertex_invalidate(se->edge.A, invalidation);

  fse = containerof(se->edge.B, typeof(*fse), vertex);
  mod = containerof(se->edge.A, typeof(*mod), vertex);

  /* update/verify cached fields */
  switch(edge_type_get(&se->edge)) {
    case EDGE_BUILD_DIR_MOD:
      RUNTIME_ASSERT(mod->build_dir == 0 || mod->build_dir == fse);
      mod->build_dir = fse;
      RUNTIME_ASSERT(vertex_type_get(&mod->build_dir->vertex) == VERTEX_DIR);
      break;
    case EDGE_ANCHOR_DIR_MOD:
      mod->anchor_dir = fse;
      RUNTIME_ASSERT(vertex_type_get(&mod->anchor_dir->vertex) == VERTEX_DIR);
      break;
    default:
      break;
  }
}

void special_edge_cleanup()
{
  special_edge *se;

  while((se = llist_shift(&special_edge_freelist, typeof(*se), edge.owner))) {
    free(se);
  }
}


void special_disconnecting(special_edge * restrict se, graph_invalidation_context * restrict invalidation)
{
  formula *fml;
  struct varset *varset;
  struct configset *configset;
  struct ruleset *ruleset;
  fsent *fse;

  if(vertex_type_get(se->edge.B) == VERTEX_CONFIGSET) {
    config_system_invalidate();
  }

  vertex_invalidate(se->edge.A, invalidation);

  fml = containerof(se->edge.A, typeof(*fml), vertex);
  varset = containerof(se->edge.A, typeof(*varset), vertex);
  configset = containerof(se->edge.A, typeof(*configset), vertex);
  ruleset = containerof(se->edge.A, typeof(*ruleset), vertex);

  fse = containerof(se->edge.B, typeof(*fse), vertex);

  /* update cached fields */
  switch(edge_type_get(&se->edge)) {
    case EDGE_ANCHOR_DIR_MOD:
    case EDGE_BUILD_DIR_MOD:
      RUNTIME_ASSERT(vertex_type_get(&fse->vertex) == VERTEX_DIR);
      break;
    case EDGE_FSENT_FML:
      fml->self_node = 0;
      break;
    case EDGE_FSENT_VARSET:
      varset->self_node = 0;
      break;
    case EDGE_FSENT_CONFIGSET:
      configset->self_node = 0;
      break;
    case EDGE_FSENT_RULESET:
      ruleset->self_node = 0;
      break;
    default:
      break;
  }
}
