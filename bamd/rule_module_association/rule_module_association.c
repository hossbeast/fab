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
#include "bacon/builder.h"

#include "rule_module_association.h"
#include "buildplan_entity.h"
#include "dependency.h"
#include "module.h"
#include "rule.h"
#include "fsent.h"
#include "fsedge.h"
#include "shadow.h"
#include "ruleset.h"
#include "vertex.h"
#include "edge.h"
#include "meta_edge.h"
#include "entlist.h"
#include "edge.h"

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
  int x;

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
      case EDGE_FSNAME_RMA:
      case EDGE_RMA_RULESET:
      case EDGE_RMA_MOD:
      case EDGE_RMA_TGT:
        entlist_push(&edges, edge);
        break;
      default:
        edge_show(edge);
        RTA;
    }
  }

  for(x = 0; x < ARRAY_LEN(rma->links); x++) {
    if(rma->links[x]) {
      entlist_push(&edges, &rma->links[x]->edge);
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
    struct rule * restrict rule
  , module * mod
  , ruleset * restrict rules
  , graph_invalidation_context * restrict invalidation
)
{
  rule_module_association *rma = 0;
  meta_edge *me;
  uint64_t u64;

  rule_module_association_alloc(&rma);

  rma->rule = rule;
  rma->mod = mod;

  meta_connect(&rule->vertex, &rma->vertex, EDGE_RULE_RMA, invalidation, &me);
  meta_connect(&rma->vertex, &mod->vertex, EDGE_RMA_MOD, invalidation, &me);
  if(rules) {
    meta_connect(&rma->vertex, &rules->vertex, EDGE_RMA_RULESET, invalidation, &me);
  }

  /* context node for running the rma */
  if(rules) {
    rma->dirnode = fsent_parent(rules->self_node);
  } else {
    rma->dirnode = rma->mod->dir_node;
  }

  u64 = 0;
  u64 += 0x7788eeaf;
  u64 = hash64(u64, rule->id, rule->id_len);
  u64 += 0xaa339aff;
  u64 = hash64(u64, mod->id, mod->id_len);
  rma->id_len = zcatf(rma->id, sizeof(rma->id), "%016"PRIx64, u64);

  vertex_invalidate(&rma->vertex, invalidation);

  /* shadow refs */
  memset(rma->links, 0, sizeof(rma->links));
  shadow_link(g_shadow_rmas, rma->id, rma->id_len, &rma->vertex, &rma->links[0]);
  if(rule->name_len) {
    shadow_link(g_shadow_rmas_byname, rule->name, rule->name_len, &rma->vertex, &rma->links[1]);
  }

  shadow_link(mod->shadow_rmas, rule->id, rule->id_len, &rma->vertex, &rma->links[2]);
  shadow_link(mod->shadow_rules, rule->id, rule->id_len, &rule->vertex, &rma->links[3]);
  if(rule->name_len) {
    shadow_link(mod->shadow_rmas_byname, rule->name, rule->name_len, &rma->vertex, &rma->links[4]);
    shadow_link(mod->shadow_rules_byname, rule->name, rule->name_len, &rule->vertex, &rma->links[5]);
  }
}

static void rma_stats_collate(rule_module_association *rma)
{
  uint32_t count;

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
  rma->rma_stats.bpe_hits = count;
}

void rma_stats_build(rule_module_association * restrict rma, bool reset, bacon_writer * restrict builder_writer)
{
  rma_stats_collate(rma);

  stats_build(&rma->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&rma->rma_stats.record, &descriptor_stats_rma, builder_writer);

  if(reset) {
    memset(&rma->stats, 0, sizeof(rma->stats));
    memset(&rma->rma_stats, 0, sizeof(rma->rma_stats));
  }
}

void rma_describe(const rule_module_association * restrict rma, struct bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;
  entlist list, *eln;

  { MAPPING
    vertex_path_describe(&rma->vertex, builder_writer);
    { SET
      vertex_mappings(&rma->vertex, builder_writer);
      moria_foreach_edge_down(&st, &rma->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_RULE_RMA:
            break;
          default:
            continue;
        }
        { MAPPING
          STRING("rule")
          vertex_path_describe(e->B, builder_writer);
          break;
        }
      }
      moria_foreach_edge_up(&st, &rma->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_RMA_MOD:
            break;
          default:
            continue;
        }
        { MAPPING
          STRING("module")
          vertex_path_describe(e->A, builder_writer);
          break;
        }
      }
      moria_foreach_edge_up(&st, &rma->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_RMA_RULESET:
            break;
          default:
            continue;
        }
        { MAPPING
          STRING("ruleset")
          vertex_path_describe(e->A, builder_writer);
          break;
        }
      }
      moria_foreach_edge_down(&st, &rma->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_FML_RMA:
            break;
          default:
            continue;
        }
        { MAPPING
          STRING("formula")
          vertex_path_describe(e->B, builder_writer);
          break;
        }
      }
      if(edges_gather_down(&rma->vertex, EDGE_DIR_RMA, &list)) {
        MAPPING STRING("dirs")
        { SET
          entlist_foreach(&list, e, eln) {
            vertex_path_describe(e->B, builder_writer);
          }
        }
        entlist_destroy(&list);
      }

      /* bindings are created by lookup/attachment rules */
      if(edges_gather_up(&rma->vertex, EDGE_RMA_TGT, &list)) {
        MAPPING STRING("bindings")
        { SET
          entlist_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        entlist_destroy(&list);
      }

      /* bpes are created by search/generate rules */
      if(edges_gather_up(&rma->vertex, EDGE_RMA_BPE, &list)) {
        MAPPING STRING("bpes")
        { SET
          entlist_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        entlist_destroy(&list);
      }
      { MAPPING STRING("stats")
        { SET
          rma_stats_build((struct rule_module_association *)rma, false, builder_writer);
        }
      }
    }
  }
}

size_t rma_path_cat(void * restrict dst, size_t sz, const rule_module_association * restrict rma)
{
  return zcatf(dst, sz, "//rmas/%.*s", rma->id_len, rma->id);
}

void rma_identity_say(const struct rule_module_association * restrict rma, narrator * restrict N)
{
  attrs32_say(rma->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s", rma->id_len, rma->id);
  if(rma->rule->name_len) {
    narrator_sayf(N, " name %.*s", rma->rule->name_len, rma->rule->name);
  }
  narrator_sayf(N, " %s [%d,%d - %d,%d]", rma->rule->loc.fname, rma->rule->loc.f_lin + 1, rma->rule->loc.f_col + 1, rma->rule->loc.l_lin + 1, rma->rule->loc.l_col + 1);
  narrator_says(N, " ");
  rule_say(rma->rule, N);
}
