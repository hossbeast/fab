/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/edge.h"
#include "moria/vertex.h"
#include "moria/connect.h"
#include "valyria/llist.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "meta_edge.h"
#include "fsent.h"
#include "fsedge.h"
#include "vertex.h"
#include "reconcile.h"
#include "edge.h"
#include "rule_module_association.h"
#include "buildplan_entity.h"
#include "formula.h"
#include "rule.h"
#include "ruleset.h"
#include "entlist.h"
#include "rule_system.h"

static llist meta_edge_list = LLIST_INITIALIZER(meta_edge_list);          // active list
static llist meta_edge_freelist = LLIST_INITIALIZER(meta_edge_freelist);  // freelist

//
// static
//

static void meta_edge_alloc(meta_edge ** restrict mep)
{
  meta_edge *se;

  if((se = llist_shift(&meta_edge_freelist, typeof(*se), edge.owner)) == 0) {
    xmalloc(&se, sizeof(*se));
  } else {
    memset(se, 0, sizeof(*se));
  }

  moria_edge_init(&se->edge, &g_graph);

  llist_append(&meta_edge_list, &se->edge.owner);
  *mep = se;
}

static void meta_edge_dispose(meta_edge * restrict se)
{
  llist_delete(&se->edge.owner);
  llist_append(&meta_edge_freelist, &se->edge.owner);
}

void meta_edge_release(meta_edge * restrict se)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(!rbnode_attached(&se->edge.rbn_up));
  RUNTIME_ASSERT(!rbnode_attached(&se->edge.rbn_down));

  meta_edge_dispose(se);
}

void meta_edge_cleanup()
{
  meta_edge *se;

  while((se = llist_shift(&freelist, typeof(*se), edge.owner))) {
    ASAN_CLEAN(se);
    free(se);
  }
}

meta_edge *meta_connect(
    moria_vertex * restrict B
  , moria_vertex * restrict A
  , edge_type type
  , graph_invalidation_context * restrict invalidation
)
{
  meta_edge *me;
  moria_edge *e;
  moria_connect_context ctx;
  int r;
  rule_module_association *rma;
  buildplan_entity *bpe;
  fsent *tgt;
  formula *fml;
  struct rule *rule;

  r = moria_preconnect(&ctx, &g_graph, B, A, type, &e);
  if(r == MORIA_HASEDGE) {
    return containerof(e, meta_edge, edge);
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  meta_edge_alloc(&me);
  moria_connect(&ctx, &g_graph, &me->edge, B, A, type);

  /* update cached fields */
  switch(type) {
    case EDGE_RULE_RULESET:
      break;
    case EDGE_RULE_RMA:
      rule = containerof(B, typeof(*rule), vertex);
      rma = containerof(A, typeof(*rma), vertex);
      me->rma = rma;
      rma->rule = rule;
      break;
    case EDGE_FML_RMA:
      fml = containerof(B, typeof(*fml), vertex);
      rma = containerof(A, typeof(*rma), vertex);
      me->fml = fml;
      me->rma = rma;
      rma->fml = fml;
      rma->fml_rma_me = me;
      break;
    case EDGE_RMA_BPE:
      rma = containerof(B, typeof(*rma), vertex);
      bpe = containerof(A, typeof(*bpe), vertex);
      me->rma = rma;
      me->bpe = bpe;
      bpe->rma = rma;
      bpe->rma_bpe_me = me;
      break;
    case EDGE_RMA_BIND:
      rma = containerof(B, typeof(*rma), vertex);
      tgt = containerof(A, typeof(*tgt), vertex);
      me->rma = rma;
      me->tgt = tgt;
      break;
    case EDGE_RMA_MOD:
    case EDGE_FSNAME_RMA:
    case EDGE_FSNAME_MOD:
    case EDGE_FSNAME_PLUGIN:
    case EDGE_NS_RMA:
    case EDGE_LOOKUP:
    case EDGE_LOOKUP_PLUGIN_FILE:
    case EDGE_FML_REC_BPE:
    case EDGE_FML_REC_BUILD_REC:
    case EDGE_FML_BPE:
      break;
    default:
      RTA;
  }

  return me;
}

void meta_disconnecting(
    meta_edge * restrict me
  , entlist * restrict edges
  , entlist * restrict vertices
  , graph_invalidation_context * restrict invalidation
)
{
  buildplan_entity *bpe;
  formula *fml;
  rule_module_association *rma;
  fsent *tgt;

  /* update cached fields */
  switch(edge_type_get(&me->edge)) {
    case EDGE_RMA_RULESET:
      rma = containerof(me->edge.B, typeof(*rma), vertex);
      rma->rules = 0;
      vertex_invalidate(me->edge.B, invalidation);
      break;
    case EDGE_RULE_RMA:
      rma = containerof(me->edge.A, typeof(*rma), vertex);
      rma->rule = 0;
      break;
    case EDGE_FML_RMA:
      RUNTIME_ASSERT(vertex_type_get(me->edge.B) == VERTEX_FORMULA);
      RUNTIME_ASSERT(vertex_type_get(me->edge.A) == VERTEX_RMA);
      fml = containerof(me->edge.B, typeof(*fml), vertex);
      rma = containerof(me->edge.A, typeof(*rma), vertex);
      rma->fml = 0;
      rma->fml_rma_me = 0;
      vertex_invalidate(me->edge.B, invalidation);
      vertex_invalidate(me->edge.A, invalidation);
      break;
    case EDGE_RMA_BPE:
      bpe = containerof(me->edge.A, typeof(*bpe), vertex);
      bpe->rma = 0;
      bpe->rma_bpe_me = 0;

      vertex_state_set(&bpe->vertex, VERTEX_UNLINKED);

      /* also consider incident EDGE_SRC_BPE edges */
      buildplan_entity_source_edges(bpe, edges);
      /* also consider incident EDGE_BPE_TGT edges */
      buildplan_entity_target_edges(bpe, edges);
      break;
    case EDGE_RMA_TGT:
      tgt = containerof(me->edge.A, typeof(*tgt), vertex);
      /* also release the target */
      entlist_push(edges, moria_edge_between(&tgt->vertex, &fsent_parent(tgt)->vertex));
    case EDGE_RMA_MOD:
    case EDGE_FSNAME_RMA:
    case EDGE_FSNAME_MOD:
    case EDGE_FSNAME_PLUGIN:
    case EDGE_FML_REC_BPE:
    case EDGE_FML_REC_BUILD_REC:
    case EDGE_FML_BPE:
      break;
    default:
      RTA;
  }
}

void meta_edge_cleanup()
{
  meta_edge *se;
  llist *T;

  llist_foreach_safe(&meta_edge_list, se, edge.owner, T) {
    meta_edge_dispose(se);
  }

  llist_foreach_safe(&meta_edge_freelist, se, edge.owner, T) {
    free(se);
  }
}
