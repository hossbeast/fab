/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "common/snarf.h"
#include "narrator/fixed.h"
#include "bacon/parser.h"
#include "bacon/builder.h"
#include "xlinux/xstdlib.h"
#include "moria/foreach.h"

#include "varset.h"
#include "fsent.h"
#include "vertex.h"
#include "mak_parser.h"
#include "edge.h"
#include "stats.h"
#include "system.h"
#include "events/events.h"
#include "edges.h"
#include "vertices.h"
#include "vertex/probes.h"
#include "disintegrate.h"

#include "zcat.h"

llist varset_list = LLIST_INITIALIZER(varset_list);                         // active
static llist varset_freelist = LLIST_INITIALIZER(varset_freelist);          // free
static llist varset_invalid_list = LLIST_INITIALIZER(varset_invalid_list);  // invalidated

static bacon_parser *parser;
static mak_parser *parser_mak;

//
// static
//

static void var_parse(varset * restrict vp)
{
  char *text = 0;
  size_t text_len;
  bacon_value * val;
  int exit;
  bamipc_message *msg;
  events_context ectx;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  moria_vertex *self;

  vp->val = 0;

  snarfs(&text, &text_len, vp->self_node_abspath);
  if(!text_len) {
    vp->varset_stats.parse_ok++;
    system_stats_events.varset_parse_ok++;
    goto end;
  }

  self = &vp->self_node->vertex;
  if(self->label_len > 4 && memncmp(self->label + self->label_len - 4, 4, MMS(".mak")) == 0) {
    exit = mak_parser_parse(parser_mak, text, text_len, vp->self_node_abspath, &val);
  } else {
    exit = bacon_parser_parse(parser, text, text_len, vp->self_node_abspath, BACON_TYPE_SET, &val);
  }

  if(exit == 0) {
    vp->varset_stats.parse_ok++;
    system_stats_events.varset_parse_ok++;
    vp->val = val;
  } else {
    vp->varset_stats.parse_err++;
    system_stats_events.varset_parse_err++;
    bacon_value_free(parser, val);

    msg = system_error_event(&ectx);
    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING STRING("varset parse error")
      { SET
        { MAPPING STRING("error")
          BYTES(parser->error, parser->error_len);
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

static void var_dispose(struct varset * restrict var)
{
  bacon_value_free(parser, var->val);

  llist_delete(&var->vertex.owner);
  llist_append(&varset_freelist, &var->vertex.owner);
  ASAN_TAINT(var);
}

void varset_release(varset * restrict var)
{
  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
  v = &var->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  var_dispose(var);
}

//
// public
//

void varset_alloc(varset ** restrict vp, moria_graph * restrict g)
{
  struct varset *var;

  if((var = llist_shift(&varset_freelist, typeof(*var), vertex.owner))) {
    ASAN_CLEAN(var);
    memset(var, 0, sizeof(*var));
  } else {
    xmalloc(&var, sizeof(*var));
  }

  moria_vertex_init(&var->vertex, g, VERTEX_VARSET | VERTEX_OK);

  llist_append(&varset_list, &var->vertex.owner);

  vertex_alloc_probe(&var->vertex);
  *vp = var;
}

void varset_reparse(varset * restrict var)
{
  if(!vertex_invalid_get(&var->vertex)) {
    return;
  }

  bacon_value_free(parser, var->val);
  var->val = 0;

  var_parse(var);
  if(!system_has_error) {
    vertex_mark_fresh(&var->vertex);
    vertex_mark_fresh(&var->self_node->vertex);
  }
}

void varset_setup()
{
  parser_mak = mak_parser_create();
  parser = parser_mak->bacon_parser;
}

void varset_cleanup()
{
  varset *v;

  while((v = llist_shift(&varset_freelist, typeof(*v), vertex.owner))) {
    ASAN_CLEAN(v);
    free(v);
  }

  bacon_parser_free(parser);
}

void varset_stats_collate(struct varset * restrict var, bool reset)
{
  if(reset) {
    memset(&var->stats, 0, sizeof(var->stats));
    memset(&var->varset_stats, 0, sizeof(var->varset_stats));
  }
}

size_t varset_path_cat(void * restrict dst, size_t sz, const varset * restrict cfg)
{
  return zcatf(dst, sz, "//bm/varsets/%.*s", cfg->id_len, cfg->id);
}

void varset_stats_build(varset * restrict varset, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&varset->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&varset->varset_stats.record, &descriptor_stats_varset, builder_writer);
}

void varset_describe(const varset * restrict varset, struct bacon_writer * restrict builder_writer)
{
  moria_foreach_state st;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&varset->vertex, builder_writer);
    { SET
      {
        vertex_mappings(&varset->vertex, builder_writer);
        { MAPPING STRING("self")
          BYTES(varset->self_node_abspath, varset->self_node_abspath_len)
        }
        { MAPPING STRING("modules")
          { LIST
            moria_foreach_edge_up(&st, &varset->vertex, e) {
              switch(edge_type_get(e)) {
                case EDGE_VARSET_MOD:
                  vertex_path_describe(e->A, builder_writer);
                default:
                  break;
              }
            }
          }
        }
        { MAPPING STRING("stats")
          { SET
            varset_stats_build((struct varset*)varset, false, builder_writer);
          }
        }
      }
      vertex_relations_describe(&varset->vertex, builder_writer);
    }
  }
}

void varset_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset, builder_writer);
}

void varset_identity_describe(const struct varset * restrict varset, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(varset->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(varset->id, varset->id_len);
  }
  { MAPPING
    STRING("path")
    STRING(varset->self_node_abspath);
  }
}

void varset_id(const struct varset * restrict varset, const char ** restrict id, uint16_t * restrict idl)
{
  *id = varset->id;
  *idl = varset->id_len;
}

void varset_compute_id(const struct varset * restrict var, struct identity_ctx * restrict id)
{
  identity_type(id, vertex_type_get(&var->vertex));
  identity_add(id, " ", 1);
  fsent_id_path_compute(var->self_node, id);
}

bool varset_reap(struct varset * restrict varset, struct graph_disintegrate_context * restrict ctx)
{
  if(edges_check_up(&varset->vertex, EDGE_VARSET_MOD)) {
    return false;
  }

  /* if no longer referenced by any module, then this varset is eligible to be reaped and any
   * of its edges will be cleaned up */
  return true;
}

void varset_unlink(struct varset * restrict varset, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  moria_foreach_edge(&st, &varset->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_FSENT_VARSET:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

#if CONFIG_DEVEL
void varsets_show(void)
{
  varset *fml;
  int x;

  printf("varsets, live : %zu free : %zu\n", llist_count(&varset_list), llist_count(&varset_freelist));
  x = 0;
  llist_foreach(&varset_list, fml, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&fml->vertex);
    vertex_edges_show(&fml->vertex);
  }
}
#endif
