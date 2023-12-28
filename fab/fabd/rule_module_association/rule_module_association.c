/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>
#include "types.h"

#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "moria/edge.h"
#include "moria/foreach.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "common/hash.h"

#include "rule_module_association.h"
#include "buildplan_entity.h"
#include "dependency.h"
#include "module.h"
#include "rule.h"
#include "fsent.h"
#include "fsedge.h"
#include "shadow.h"
#include "rules.h"
#include "vertex.h"
#include "edge.h"
#include "meta_edge.h"
#include "entlist.h"

#include "zcat.h"
#include "atomics.h"

llist rma_list = LLIST_INITIALIZER(rma_list);
static llist rma_freelist = LLIST_INITIALIZER(rma_freelist);

//
// static
//

void rule_module_association_init(rule_module_association * restrict rma)
{
  memset(rma, 0, sizeof(*rma));

  moria_vertex_init(&rma->vertex, &g_graph, VERTEX_RMA | VERTEX_OK);
  llist_init(&rma->work.changed[0].lln);
  llist_init(&rma->work.changed[1].lln);
//  rma->work.mark = VERTEX_RMA;
}

void rule_module_association_alloc(rule_module_association ** restrict rmap)
{
  rule_module_association *rma;

  if(!(rma = llist_shift(&rma_freelist, typeof(*rma), vertex.owner))) {
    xmalloc(&rma, sizeof(*rma));
  }

  rule_module_association_init(rma);

  llist_append(&rma_list, &rma->vertex.owner);
  *rmap = rma;
}

static void rule_module_association_dispose(rule_module_association * restrict rma)
{
  llist_delete(&rma->vertex.owner);
  llist_push(&rma_freelist, &rma->vertex.owner);
}

//
// public
//

void rule_module_association_cleanup(void)
{
  llist *T;
  rule_module_association *rma;

  llist_foreach_safe(&rma_list, rma, vertex.owner, T) {
    rule_module_association_dispose(rma);
  }

  llist_foreach_safe(&rma_freelist, rma, vertex.owner, T) {
    free(rma);
  }
}

void rule_module_association_release(rule_module_association * restrict rma, graph_invalidation_context * restrict invalidation)
{
  moria_vertex *v;
  moria_edge *edge;
  moria_foreach_state st;
  entlist edges, vertices;

  /* proactive edge disconnect */
  entlist_init(&vertices);
  entlist_init(&edges);
  moria_foreach_edge(&st, &rma->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_BPE:
        entlist_push(&vertices, edge->A);
        entlist_push(&edges, edge);
        break;
      case EDGE_RULE_RMA:
      case EDGE_FML_RMA:
      case EDGE_DIR_RMA:
      case EDGE_INDEX_RMA:
      case EDGE_RMA_RULES:
      case EDGE_RMA_MOD:
      case EDGE_RMA_TGT:
        entlist_push(&edges, edge);
        break;
      default:
        edge_show(edge);
        RTA;
    }
  }

  graph_disintegrate_bulk(&edges, &vertices, invalidation);
  entlist_destroy(&edges);
  entlist_destroy(&vertices);

  /*
   * Remove this rma from the run-queue. It might be in the rma run queue because the rule was attached
   * to a directory which was changed while unloading the module
   */
  llist_delete(&rma->work.changed[0].lln);
  llist_delete(&rma->work.changed[1].lln);

  /* should be fully excised from the graph previously */
  v = &rma->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  rule_module_association_dispose(rma);
}

void rule_module_association_create(
    rule * restrict rule
  , module * mod
  , rules * restrict rules
  , graph_invalidation_context * restrict invalidation
)
{
  rule_module_association *rma = 0;
  meta_edge *me;
  uint64_t u64;
  uint64_t token;

  rule_module_association_alloc(&rma);
  meta_connect(&rule->vertex, &rma->vertex, EDGE_RULE_RMA, invalidation, &me);
  meta_connect(&rma->vertex, &mod->vertex, EDGE_RMA_MOD, invalidation, &me);
  if(rules) {
    meta_connect(&rma->vertex, &rules->vertex, EDGE_RMA_RULES, invalidation, &me);
  }

  /* context node for running the rma */
  if(rules) {
    rma->dirnode = fsent_parent(rules->self_node);
  } else {
    rma->dirnode = rma->mod->dir_node;
  }

  u64 = 0;
  u64 = hash64(u64, rule->id, rule->id_len);
  u64 = hash64(u64, mod->id, mod->id_len);
  rma->id_len = zcatf(rma->id, sizeof(rma->id), "%016"PRIx64, u64);
  token = atomic_fetch_inc(&vertex_token_counter);
  rma->token_len = zcatf(rma->token, sizeof(rma->token), "%016"PRIx64, hash64(u64, MM(token)));

  vertex_invalidate(&rma->vertex, invalidation);
}

void rma_mod_set(rule_module_association * restrict rma, module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  fsent *shadow;
  moria_edge *edge;

  if(mod)
  {
    shadow_create(mod->shadow_rma, &shadow, rma->rule->id, rma->rule->id_len, VERTEX_SHADOW_RULE, invalidation);
    shadow->self_rma = rma;
    edge = moria_edge_between(&shadow->vertex, &fsent_parent(shadow)->vertex);
    rma->shadow = containerof(edge, fsedge, edge);
  }
  else if(rma->shadow)
  {
    graph_disintegrate(&rma->shadow->edge, invalidation);
  }
  rma->mod = mod;
}

size_t rule_module_association_stats_collate(void *dst, size_t sz, bool reset, rule_module_association *rma)
{
  size_t z;
  bam_rule_stats *stats;
  bam_rule_stats tmp;
  uint32_t count;

  memcpy(&tmp, &rma->stats, sizeof(tmp));
  if(reset) {
    memset(&rma->stats, 0, sizeof(rma->stats));
  }
  stats = &tmp;

  /* dependencies */
  count = 0;
  moria_traverse_vertices(&g_graph, &rma->vertex, moria_vertex_count, 0
    , (moria_traversal_criteria[]) {{
          edge_travel_mask : EDGE_TYPE_OPT
        , edge_travel : EDGE_RMA_BPE
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &count
  );
  stats->dependencies = count;

  z = 0;
  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_rule_stats, &stats->record, 0);

  return z;
}
