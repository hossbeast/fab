/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "xlinux/xstdlib.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "narrator/fixed.h"
#include "moria.h"
#include "moria/foreach.h"
#include "bacon/writer.h"
#include "bacon/value.h"
#include "bacon/builder.h"

#include "rma.h"
#include "edge.h"
#include "edges.h"
#include "vertices.h"
#include "vertex.h"
#include "events/events.h"
#include "exec_value.h"
#include "formula.h"
#include "fsedge.h"
#include "fsname.h"
#include "meta_edge.h"
#include "namespace.h"
#include "rule.h"
#include "ruleset.h"
#include "shadow.h"
#include "system.h"
#include "lookup.h"
#include "graph.h"
#include "disintegrate.h"
#include "zcat.h"
#include "identity.h"
#include "pattern.h"

llist rma_list = LLIST_INITIALIZER(rma_list);
static llist rma_freelist = LLIST_INITIALIZER(rma_freelist);

static void lookup_connect(lookup_context * restrict ctx, moria_vertex * restrict v, graph_invalidation_context * restrict invalidation)
{
  meta_edge *me;

  /* edge on the fsname visited */
  if(ctx->hit) {
    me = meta_connect(&ctx->hit->vertex, v, EDGE_FSNAME_RMA, invalidation);
    me->epoch = rule_system_epoch;
  }
  if(ctx->miss) {
    me = meta_connect(&ctx->miss->vertex, v, EDGE_FSNAME_RMA, invalidation);
    me->epoch = rule_system_epoch;
  }

  /* edge on the target */
  me = meta_connect(ctx->rctx.vertices[0], v, EDGE_LOOKUP, invalidation);
  me->epoch = rule_system_epoch;
}

static void rma_lookup_error_build(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { MAPPING
    if(ctx->rctx.ref->bound) {
      STRING("unmatched lookup")
    } else if(ctx->rctx.r > 1) {
      STRING("ambiguous lookup")
    } else {
      STRING("unresolved lookup")
    }

    { SET
      lookup_error_mappings(builder_writer, ctx);
    }
  }
}

static void lookup_error(const lookup_context * restrict ctx)
{
  narrator_fixed fixed;
  bacon_writer writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);

  bacon_writer_open(&writer, &fixed.base);
  rma_lookup_error_build(&writer, ctx);
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void resolve_lookup(
    rule_module_association * restrict rma
  , bacon_value * restrict vars
  , bacon_value * restrict val
  , graph_invalidation_context * restrict invalidation
)
{
  lookup_context ctx;
  exec_value *ev;

  if(val->type == BACON_TYPE_MAPPING) {
    val = val->val;
  }
  if((int)val->type != EXEC_VALUE_LOOKUP) {
    return;
  }

  ev = containerof(val, typeof(*ev), bacon);
  memset(&ctx, 0, sizeof(ctx));
  ctx.rctx.base = rma->dirnode;
  ctx.rctx.vars = vars;
  ctx.rctx.ref = &ev->lookup.pattern->pattern;
  ctx.loc = &ctx.rctx.ref->loc;
  ctx.ns_name = ev->lookup.namespace;
  ctx.ns_name_len = ev->lookup.namespace_len;
  if(lookup_resolve(&ctx, ev->lookup.pattern) != 1) {
    lookup_error(&ctx);
    return;
  }

  lookup_connect(&ctx, &rma->vertex, invalidation);
  lookup_store_put(&rma->lookups, ev, ctx.rctx.vertices[0]);
}

static void rule_module_association_dispose(rule_module_association * restrict rma)
{
  lookup_store_destroy(&rma->lookups);

  llist_delete(&rma->vertex.owner);
  llist_push(&rma_freelist, &rma->vertex.owner);
  ASAN_TAINT(rma);
}

static void rma_stats_collate(rule_module_association *rma)
{
  rma->rma_stats.bpes = edges_count_up(&rma->vertex, EDGE_RMA_BPE);
  rma->rma_stats.dirs = edges_count_down(&rma->vertex, EDGE_DIR_RMA);
}

void rma_resolve(struct rule_module_association * restrict rma)
{
  bacon_value *vars;
  bacon_value *val;
  graph_invalidation_context invalidation = {};

  if(!rma->fml) {
    return;
  }

  graph_invalidation_begin(&invalidation);

  vars = 0;
  if(rma->mod) {
    vars = rma->mod->vars;
  }

  if(rma->fml->file) {
    resolve_lookup(rma, vars, rma->fml->file, &invalidation);
    if(system_has_error) {
      goto end;
    }
  }
  if(rma->fml->args) {
    llist_foreach(&rma->fml->args->list, val, lln) {
      resolve_lookup(rma, vars, val, &invalidation);
      if(system_has_error) {
        goto end;
      }
    }
  }
  if(rma->fml->envs) {
    rbtree_foreach(&rma->fml->envs->set, val, rbn) {
      resolve_lookup(rma, vars, val, &invalidation);
      if(system_has_error) {
        goto end;
      }
    }
  }

end:
  graph_invalidation_end(&invalidation);
}

void rule_module_association_init(rule_module_association * restrict rma)
{
  memset(rma, 0, sizeof(*rma));

  moria_vertex_init(&rma->vertex, &g_graph, VERTEX_RMA | VERTEX_OK);
  llist_init(&rma->work.changed.lln);
  lookup_store_init(&rma->lookups);
}

static void rma_alloc(rule_module_association ** restrict rmap)
{
  rule_module_association *rma;

  if((rma = llist_shift(&rma_freelist, typeof(*rma), vertex.owner))) {
    ASAN_CLEAN(rma);
    memset(rma, 0, sizeof(*rma));
  } else {
    xmalloc(&rma, sizeof(*rma));
  }

  rule_module_association_init(rma);

  llist_append(&rma_list, &rma->vertex.owner);
  *rmap = rma;
}

void rma_unlink(rule_module_association * restrict rma, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  /*
   * Remove this rma from the run-queue. It might be in the rma run queue because the rule was attached
   * to a directory which was changed while unloading the module
   */
  llist_delete(&rma->work.changed.lln);

  /* when an rma is unlinked, also unlink (upwards) its bpes and bind targets */
  moria_foreach_edge(&st, &rma->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_RMA_BPE:
      case EDGE_RMA_BIND:
        vertices_push(ctx->unlinking, edge->A);
      case EDGE_DIR_RMA:
      case EDGE_RULE_RMA:
      case EDGE_RMA_MOD:
      case EDGE_FML_RMA:
      case EDGE_FSNAME_RMA:
      case EDGE_LOOKUP:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

void rma_release(rule_module_association * restrict rma)
{
  moria_vertex *v;

  /* it could have been re-enqueued between unlink and release */
  llist_delete(&rma->work.changed.lln);

  /* should be fully excised from the graph previously */
  v = &rma->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  rule_module_association_dispose(rma);
}

void rma_create(
    struct rule * restrict rule
  , module * mod
  , ruleset * restrict rules
  , graph_invalidation_context * restrict invalidation
)
{
  rule_module_association *rma = 0;
  identity_ctx id;

  rma_alloc(&rma);

  rma->rule = rule;
  rma->mod = mod;

  meta_connect(&rule->vertex, &rma->vertex, EDGE_RULE_RMA, invalidation);
  meta_connect(&rma->vertex, &mod->vertex, EDGE_RMA_MOD, invalidation);
  meta_connect(&rule->vertex, &rules->vertex, EDGE_RULE_RULESET, invalidation);

  /* context node for running the rma */
  rma->dirnode = fsent_parent(rules->self_node);

  identity_init(&id);
  rma_compute_id(rma, &id);
  rma->id_len = identity_mk(&id, rma->id, sizeof(rma->id));

  vertex_invalidate(&rma->vertex, invalidation);

  /* global links to the rma */
  shadow_mkref_new(g_shadow_rmas, rma->id, rma->id_len, &rma->vertex);
  if(rule->name_len) {
    shadow_mkref(g_shadow_rmas_byname, rule->name, rule->name_len, &rma->vertex);
  }

  /* module links to the rma */
  shadow_mkref_new(mod->shadow_rmas, rma->id, rma->id_len, &rma->vertex);
  if(rule->name_len) {
    shadow_mkref(mod->shadow_rmas_byname, rule->name, rule->name_len, &rma->vertex);
  }

  /* module links to the rule */
  shadow_mkref_new(mod->shadow_rules, rule->id, rule->id_len, &rma->vertex);
  if(rule->name_len) {
    shadow_mkref(mod->shadow_rules_byname, rule->name, rule->name_len, &rma->vertex);
  }
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

void rma_stats_describe(const rule_module_association * restrict rma, bacon_writer * restrict builder_writer)
{
  { SET
    rma_stats_build((void*)rma, false, builder_writer);
  }
}

void rma_stats_say(const rule_module_association * restrict rma, narrator * restrict N)
{
  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, N);

  rma_stats_describe(rma, writer);

  bacon_writer_close(writer);
  bacon_writer_free(writer);
}

void rma_stats_show(const rule_module_association * restrict rma)
{
  rma_stats_say(rma, g_narrator_stdout);
  printf("\n");
}

void rma_describe(const rule_module_association * restrict rma, struct bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&rma->vertex, builder_writer);
    { SET
      vertex_mappings(&rma->vertex, builder_writer);
      { MAPPING STRING("name")
        if(rma->rule && rma->rule->name_len) {
          BYTES(rma->rule->name, rma->rule->name_len)
        } else {
          STRING("_")
        }
      }
      moria_foreach_edge_down(&st, &rma->vertex, e) {
        switch(edge_type_get(e)) {
          case EDGE_RULE_RMA:
            break;
          default:
            continue;
        }
        { MAPPING STRING("rule")
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
        { MAPPING STRING("module")
          vertex_path_describe(e->A, builder_writer);
          break;
        }
      }
      { MAPPING STRING("stats")
        { SET
          rma_stats_build((struct rule_module_association *)rma, false, builder_writer);
        }
      }

      vertex_relations_describe(&rma->vertex, builder_writer);
    }
  }
}

size_t rma_path_cat(void * restrict dst, size_t sz, const rule_module_association * restrict rma)
{
  return zcatf(dst, sz, "//bm/rmas/%.*s", rma->id_len, rma->id);
}

void rma_identity_describe(const struct rule_module_association * restrict rma, bacon_writer * restrict builder_writer)
{
  attrs32_describe(rma->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING STRING("id")
    BYTES(rma->id, rma->id_len);
  }
  if(rma->rule && rma->rule->name_len) {
    MAPPING STRING("name")
    BYTES(rma->rule->name, rma->rule->name_len);
  }
  { MAPPING STRING("rule")
    if(rma->rule) {
      BYTES(rma->rule->id, rma->rule->id_len);
    } else {
      STRING("_")
    }
  }
  { MAPPING STRING("module")
    BYTES(rma->mod->id, rma->mod->id_len);
  }
  if(rma->rule) {
    /* location and contents */
    bacon_writer_push_string(builder_writer);
    narrator_says(builder_writer->N, rma->rule->loc.fname);
    narrator_sayf(builder_writer->N, " [%d,%d - %d,%d] ", rma->rule->loc.f_lin + 1, rma->rule->loc.f_col + 1, rma->rule->loc.l_lin + 1, rma->rule->loc.l_col + 1);
    rule_say(rma->rule, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
  }
}

void rma_reset(rule_module_association * restrict rma)
{
  lookup_store_reset(&rma->lookups);

  rma->rma_stats.target_hits = 0;
  rma->rma_stats.attachment_hits = 0;
  rma->rma_stats.search_hits = 0;
  rma->rma_stats.generate_hits = 0;
  rma->rma_stats.formula_hits = 0;
}

void rule_module_association_cleanup(void)
{
  rule_module_association *rma;

  while((rma = llist_shift(&rma_freelist, typeof(*rma), vertex.owner))) {
    ASAN_CLEAN(rma);
    free(rma);
  }
}

void rma_id(const struct rule_module_association * restrict rma, const char ** restrict id, uint16_t * restrict idl)
{
  *id = rma->id;
  *idl = rma->id_len;
}

void rma_compute_id(const rule_module_association * restrict rma, identity_ctx * restrict id)
{
  /* rule */
  vertex_compute_id(&rma->rule->vertex, id);
  identity_add(id, "\n", 1);

  /* module */
  vertex_compute_id(&rma->mod->vertex, id);
  identity_add(id, "\n", 1);

  /* rma */
  identity_type(id, VERTEX_RMA);
}

#if CONFIG_DEVEL
void rmas_show(void)
{
  struct rule_module_association *rma;
  int x = 0;

  printf("rmas, live : %zu free : %zu\n", llist_count(&rma_list), llist_count(&rma_freelist));
  llist_foreach(&rma_list, rma, vertex.owner) {
    printf("%3u %p", x++, rma);
    printf("\n");
  }
}
#endif
