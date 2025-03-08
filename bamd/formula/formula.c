/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "moria/foreach.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"

#include "formula.h"
#include "formula_parser.h"
#include "exec_value.h"
#include "fsent.h"
#include "vertex.h"
#include "rma.h"
#include "edge.h"
#include "stats.h"
#include "marshal.h"
#include "channel.h"
#include "system.h"
#include "events/events.h"
#include "formula_parser.internal.h"
#include "selector_parser.h"
#include "reconcile.h"
#include "meta_edge.h"
#include "special_edge.h"
#include "edges.h"
#include "vertices.h"
#include "vertex/probes.h"
#include "pattern.h"
#include "rule.h"
#include "resolve.h"
#include "namespace.h"
#include "fsname.h"
#include "dir_rma_edge.h"
#include "disintegrate.h"

#include "common/snarf.h"
#include "zcat.h"

llist formula_list = LLIST_INITIALIZER(formula_list);                         // active
static llist formula_freelist = LLIST_INITIALIZER(formula_freelist);          // free
static llist formula_unref_list = LLIST_INITIALIZER(formula_unref_list);      // unreferenced

static formula_parser *parser;

static void formula_parse(formula * restrict fml)
{
  char *text = 0;
  size_t text_len;
  int exit;
  events_context ectx;
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  // open the file, both to read its contents, and to exec later
  ixclose(&fml->fd);
  fml->fd = xopenats(O_RDONLY, 0, fml->abspath);

  bacon_value_ifree(parser->bacon_parser, &fml->file);
  bacon_value_ifree(parser->bacon_parser, &fml->envs);
  bacon_value_ifree(parser->bacon_parser, &fml->args);

  fsnarf(&text, &text_len, fml->fd);

  if(!text_len) {
    fml->formula_stats.parse_ok++;
    system_stats_events.formula_parse_ok++;
    goto end;
  }

  if((exit = formula_parser_parse(parser, text, text_len, fml->abspath, fml)) == 0)
  {
    fml->formula_stats.parse_ok++;
    system_stats_events.formula_parse_ok++;
  }
  else
  {
    fml->formula_stats.parse_err++;
    system_stats_events.formula_parse_err++;

    msg = system_error_event(&ectx);
    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING STRING("formula parse error")
      { SET
        { MAPPING STRING("error")
          BYTES(parser->error, parser->error_len);
        }
        { MAPPING STRING("location")
          STRING(fml->abspath)
        }
      }
    }
    bacon_writer_close(&writer);

    msg->code = EINVAL;
    msg->len = fixed.l;
    events_publish(&ectx);
  }

end:
  free(text);
}

static void formula_destroy(formula * restrict fml)
{
  xclose(fml->fd);
  bacon_value_free(parser->bacon_parser, fml->file);
  bacon_value_free(parser->bacon_parser, fml->envs);
  bacon_value_free(parser->bacon_parser, fml->args);
}

static void formula_dispose(formula * restrict fml)
{
  formula_destroy(fml);

  llist_delete(&fml->vertex.owner);
  llist_append(&formula_freelist, &fml->vertex.owner);
  ASAN_TAINT(fml);
}

void formula_unlink(formula * restrict fml, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  llist_delete(&fml->unref_lln);

  moria_foreach_edge(&st, &fml->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_FML_REC_BPE:
      case EDGE_FML_REC_BUILD_REC:
        vertices_push(ctx->unlinking, edge->A);
      case EDGE_FML_RMA:
      case EDGE_FSENT_FML:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

void formula_release(formula * restrict fml)
{
  moria_vertex *v;

  v = &fml->vertex;

  /* must have been fully excised from the graph previously */
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  formula_dispose(fml);
}

//
// public
//

void formula_setup()
{
  formula_parser_create(&parser);
}

void formula_cleanup()
{
  formula *fml;

  while((fml = llist_shift(&formula_freelist, typeof(*fml), vertex.owner))) {
    ASAN_CLEAN(fml);
    free(fml);
  }

  formula_parser_free(parser);
  parser = 0;
}

void formula_create(formula ** restrict fmlp, moria_graph * restrict g)
{
  formula *fml;

  if((fml = llist_shift(&formula_freelist, typeof(*fml), vertex.owner))) {
    ASAN_CLEAN(fml);
    memset(fml, 0, sizeof(*fml));
  } else {
    xmalloc(&fml, sizeof(*fml));
  }

  moria_vertex_init(&fml->vertex, g, VERTEX_FORMULA | VERTEX_OK);
  llist_append(&formula_list, &fml->vertex.owner);
  llist_init(&fml->unref_lln);

  fml->fd = -1;

  vertex_alloc_probe(&fml->vertex);
  *fmlp = fml;
}

void formula_stats_build(formula * restrict fml, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&fml->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&fml->formula_stats.record, &descriptor_stats_formula, builder_writer);

  if(reset) {
    memset(&fml->stats, 0, sizeof(fml->stats));
    memset(&fml->formula_stats, 0, sizeof(fml->formula_stats));
  }
}

void formula_describe(const formula * restrict formula, bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&formula->vertex, builder_writer);
    { SET
      vertex_mappings(&formula->vertex, builder_writer);
      { MAPPING STRING("self")
        BYTES(formula->abspath, formula->abspath_len)
      }
      if(formula->file) {
        { MAPPING STRING("file")
          bacon_writer_value(builder_writer, formula->file);
        }
      }
      if(formula->args) {
        { MAPPING STRING("args")
          bacon_writer_value(builder_writer, formula->args);
        }
      }
      if(formula->envs) {
        { MAPPING STRING("envs")
          bacon_writer_value(builder_writer, formula->envs);
        }
      }
      { MAPPING STRING("rmas")
        { LIST
          moria_foreach_edge_up(&st, &formula->vertex, e) {
            switch(edge_type_get(e)) {
              case EDGE_FML_RMA:
                vertex_path_describe(e->A, builder_writer);
              default:
                break;
            }
          }
        }
      }
      { MAPPING STRING("stats")
        { SET
          formula_stats_build((struct formula *)formula, false, builder_writer);
        }
      }
      vertex_relations_describe(&formula->vertex, builder_writer);
    }
  }
}

void formula_bacon_describe(const formula * restrict formula, bacon_writer * restrict builder_writer)
{
  if(formula->file) {
    MAPPING STRING("file")
    bacon_writer_value(builder_writer, formula->file);
  }
  if(formula->args) {
    MAPPING STRING("args")
    bacon_writer_value(builder_writer, formula->args);
  }
  if(formula->envs) {
    MAPPING STRING("envs")
    bacon_writer_value(builder_writer, formula->envs);
  }
}

size_t formula_path_cat(void * restrict dst, size_t sz, const formula * restrict formula)
{
  return zcatf(dst, sz, "//bm/formulas/%.*s", formula->id_len, formula->id);
}

void formula_identity_describe(const struct formula * restrict formula, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(formula->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(formula->id, formula->id_len);
  }
  { MAPPING
    STRING("path")
    STRING(formula->abspath);
  }
}

static void namespace_resolve_error(struct lookup_context * restrict ctx)
{
  events_context ectx;
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    if(ctx->rctx.r == 0) {
      STRING("unresolved formula ref")
    } else {
      STRING("ambiguous formula ref")
    }
    lookup_namespace_error_build(builder_writer, ctx);
  }
  bacon_writer_close(&writer);

  msg->len = fixed.l;
  msg->code = EINVAL;
  events_publish(&ectx);
}

static void pattern_lookup_error(const lookup_pattern * restrict ref)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING STRING("unmatched formula ref")
    lookup_bound_error_build(builder_writer, ref);
  }

  bacon_writer_close(&writer);
  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

formula *formula_resolve(lookup_pattern * restrict ref, rule_run_context * restrict ctx)
{
  fsent *fml_n;
  formula *fml;
  moria_vertex *v;
  namespace *ns;
  meta_edge *me;
  int r;

  /* formulas is the default namespace for formula resolution */
  RUNTIME_ASSERT(namespace_formulas);
  ns = namespace_formulas;

  if(ref->pattern.bound) {
    memset(&ctx->lctx.rctx, 0, sizeof(ctx->lctx.rctx));
    ctx->lctx.rctx.mod = ctx->rma->mod;
    ctx->lctx.rctx.vars = ctx->rma->mod->vars;
    ctx->lctx.rctx.base = ctx->rma->dirnode;
    r = pattern_resolve_lookup(&ctx->lctx.rctx, ref);

    if(r == 1) {
      v = ctx->lctx.rctx.vertices[0];
    } else {
      pattern_lookup_error(ref);
      return 0;
    }
  } else {
    memset(&ctx->lctx, 0, sizeof(ctx->lctx));
    ctx->lctx.loc = &ref->pattern.loc;
    ctx->lctx.ns = ns;
    ctx->lctx.ns_name = ns->name;
    ctx->lctx.ns_name_len = ns->name_len;
    ctx->lctx.rctx.vars = ctx->rma->mod->vars;
    r = namespace_resolve(&ctx->lctx, ref);

    /* take an edge the fsname visited */
    if(ctx->lctx.hit) {
      me = meta_connect(&ctx->lctx.hit->vertex, &ctx->rma->vertex, EDGE_FSNAME_RMA, &ctx->invalidation);
      me->epoch = rule_system_epoch;
    } else if(ctx->lctx.miss) {
      me = meta_connect(&ctx->lctx.miss->vertex, &ctx->rma->vertex, EDGE_FSNAME_RMA, &ctx->invalidation);
      me->epoch = rule_system_epoch;
    }

    if(r == 1) {
      v = ctx->lctx.rctx.vertices[0];
    } else {
      namespace_resolve_error(&ctx->lctx);
      return 0;
    }
  }

  fml_n = containerof(v, typeof(*fml_n), vertex);
  fml = fsent_formula_bootstrap(fml_n, &ctx->invalidation);

  /* fml -> rma meta-edge */
  me = meta_connect(&fml->vertex, &ctx->rma->vertex, EDGE_FML_RMA, &ctx->invalidation);
  me->epoch = rule_system_epoch;

  return fml;
}

void formula_reparse(formula * restrict fml)
{
  if(!vertex_invalid_get(&fml->vertex)) {
    return;
  }

  formula_parse(fml);
  if(!system_has_error) {
    vertex_mark_fresh(&fml->vertex);
    vertex_mark_fresh(&fml->self_node->vertex);
  }
}

void formula_rma_unref(formula * restrict fml)
{
  if(llist_attached(&fml->unref_lln)) {
    return;
  }

  llist_push(&formula_unref_list, &fml->unref_lln);
}

bool formula_reap(formula * restrict fml, struct graph_disintegrate_context * restrict ctx)
{
  if(edges_check_up(&fml->vertex, EDGE_FML_RMA)) {
    return false;
  }

  /* if no longer referenced by any rma, then this formula is eligible to be reaped and any
   * of its edges will be cleaned up */
  return true;
}

void formula_id(const formula * restrict fml, const char ** restrict id, uint16_t * restrict idl)
{
  *id = fml->id;
  *idl = fml->id_len;
}

void formula_compute_id(const formula * restrict fml, identity_ctx * restrict id)
{
  identity_type(id, vertex_type_get(&fml->vertex));
  identity_add(id, " ", 1);

  /* path */
  fsent_id_path_compute(fml->self_node, id);
}

#if CONFIG_DEVEL
void formulas_show(void)
{
  formula *fml;
  int x = 0;

  printf("formulas, live : %zu free : %zu\n", llist_count(&formula_list), llist_count(&formula_freelist));
  llist_foreach(&formula_list, fml, vertex.owner) {
    printf("%3u %p", x++, fml);
    printf("\n");
  }
}
#endif
