/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "moria/foreach.h"
#include "bam/describe.desc.h"
#include "bam/ipc.h"
#include "bam/list.desc.h"
#include "bam/config_describe.desc.h"
#include "bam/vars_describe.desc.h"
#include "bam/rules_describe.desc.h"
#include "bam/rule_info.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/builder.h"
#include "bam/vertex.h"
#include "common/hash.h"

#include "handler.h"
#include "filesystem.h"
#include "config.h"
#include "configuration.h"
#include "events.h"
#include "goals.h"
#include "metadata.h"
#include "module.h"
#include "meta_edge.h"
#include "request.h"
#include "request_parser.h"
#include "stats.h"
#include "dependency.h"
#include "buildplan_entity.h"
#include "channel.h"
#include "run_thread.h"
#include "rule_module_association.h"
#include "vertex.h"
#include "dir_rma.h"
#include "exec.h"
#include "var.h"
#include "edge.h"
#include "formula.h"
#include "rules.h"
#include "fsent.h"
#include "shadow.h"
#include "buildplan.h"

#include "marshal.h"
#include "zcat.h"

struct trylock handler_build_lock;

static llist context_freelist = LLIST_INITIALIZER(context_freelist);
rcu_list g_handlers = RCU_LIST_INITIALIZER(g_handlers);

//
// handlers
//

static void handler_cmd_context(handler_context * restrict ctx, command * restrict cmd)
{
  moria_vertex *v;
  fsent *fse;
  bamipc_message *msg;
  char module_id[64];

  snprintf(module_id, sizeof(module_id), "%016"PRIx64, cmd->context.module_id);
  if((v = moria_vertex_downs(&g_shadow_modules->vertex, module_id)) == 0) {
    msg = channel_produce(ctx->chan);
    msg->code = -1;
    msg->type = BAMIPC_MSG_RESPONSE;
    msg->size = zcatf(msg->text, sizeof(msg->text), "module not found %s", module_id);
    channel_post(ctx->chan, msg);
    return;
  }

  fse = containerof(v, typeof(*fse), vertex);
  ctx->mod = fse->self_mod;
}

static void handler_select(handler_context * restrict ctx, command * restrict cmd)
{
  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.mod = ctx->mod;
  ctx->sel_ctx.cwd = g_cwd;
  ctx->sel_ctx.chan = ctx->chan;

  selector_exec(cmd->selector, &ctx->sel_ctx, SELECTION_ITERATION_TYPE_ORDER);

  // return the active selection
  ctx->selection = ctx->sel_ctx.selection;
}

static void handler_reset_selection(handler_context * restrict ctx, command * restrict cmd)
{
  ctx->selection = 0;
}

static void handler_list(handler_context * restrict ctx, command * restrict cmd)
{
  selected *sn;
  moria_vertex *v;
  fsent *n;
  size_t z;
  bamipc_message *msg;
  bam_vertex_state state;
  bam_vertex_type type;
  char *dst;
  size_t sz;
  uint16_t len;
  uint16_t off;

  llist_foreach(&ctx->selection->list, sn, lln) {
    v = sn->v;
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;

    dst = msg->text;
    sz = sizeof(msg->text);

    z = 0;
    z += marshal_u32(dst + z, sz - z, descriptor_bam_list_item.id);

    /* type */
    type = vertex_type_remap(vertex_type_get(v));
    z += marshal_int(dst + z, sz - z, &type, sizeof(type));

    /* state */
    state = vertex_bam_vertex_state_get(v);
    z += marshal_int(dst + z, sz - z, &state, sizeof(state));

    /* label */
    z += marshal_u16(dst + z, sz - z, v->label_len);
    z += zcatw(dst + z, sz - z, v->label, v->label_len);

    /* abspath */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    if(vertex_kind_get(v) == VERTEX_KIND_FSENT)
    {
      n = containerof(v, typeof(*n), vertex);
      len = fsent_absolute_path_cat(dst + z, sz - z, n);
      z += len;
      marshal_u16(dst + off, sz - off, len);
    }

    /* ref */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    switch(vertex_type_get(v)) {
      case VERTEX_SYMLINK:
      case VERTEX_SHADOW_LINK:
        if(v->ref) {
          len = fsent_absolute_path_cat(dst + z, sz - z, containerof(v->ref, fsent, vertex));
          z += len;
          marshal_u16(dst + off, sz - off, len);
        }
        break;
      case VERTEX_SHADOW_MODULE:
        n = containerof(v, typeof(*n), vertex);
        len = fsent_absolute_path_cat(dst + z, sz - z, n->self_mod->dir_node);
        z += len;
        marshal_u16(dst + off, sz - off, len);
        break;
      default:
    }

    /* distance */
    z += marshal_u16(dst + z, sz - z, sn->rank);

    msg->size = z;
    channel_post(ctx->chan, msg);
  }
}

static void handler_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  selected *sn;

  llist_foreach(&ctx->selection->list, sn, lln) {
    vertex_invalidate(sn->v, &ctx->invalidation);
  }
}

static void handler_global_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  channel *chan;
  bamipc_message *msg;

  fsent_valid_epoch++;
  ctx->invalidation.any = true;

  if(events_would(BAMIPC_EVENT_GLOBAL_INVALIDATE, &chan, &msg)) {
    events_publish(chan, msg);
  }
}

static void describe_fsent(fsent * restrict fse, uint16_t distance, bacon_writer *builder_writer)
{
  bam_vertex_state state;
  bam_vertex_type type;
  const filesystem *fs;
  char buf[PATH_MAX];
  size_t sz;

  fs = fsent_filesystem_get(fse);

  { MAPPING
    bacon_writer_push_string(builder_writer);
    fsent_absolute_path_say(fse, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      { MAPPING
        STRING("label")
        BYTES(fse->vertex.label, fse->vertex.label_len)
      }
      { MAPPING
        STRING("id")
        BYTES(fse->id, fse->id_len)
      }
      { MAPPING
        STRING("token")
        BYTES(fse->token, fse->token_len)
      }
      { MAPPING
        STRING("type")
        type = vertex_type_remap(vertex_type_get(&fse->vertex));
        STRING(attrs16_name_byvalue(bam_vertex_type_attrs, type));
      }
      { MAPPING
        STRING("state")
        state = vertex_bam_vertex_state_get(&fse->vertex);
        STRING(attrs16_name_byvalue(bam_vertex_state_attrs, state));
      }
      { MAPPING
        STRING("fsroot")
        sz = filesystem_absolute_path_cat(buf, sizeof(buf), fs);
        BYTES(buf, sz)
      }
      { MAPPING
        STRING("indexed-lookup")
        BOOL(fse->ie)
      }
      { MAPPING
        STRING("indexed-notify")
        BOOL(fse->in_wd != -1)
      }
      { MAPPING
        STRING("invalidation")
        STRING(attrs16_name_byvalue(invalidate_attrs, fs->invalidate));
      }
      if(fse->bpe && fse->bpe->fml) {
        MAPPING
        STRING("formula")

        bacon_writer_push_string(builder_writer);
        fsent_absolute_path_say(fse->bpe->fml->self_node, builder_writer->N);
        bacon_writer_pop_string(builder_writer);
      }
    }
  }
}

static void handler_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  selected *sn;
  fsent *fse;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { SET
    llist_foreach(&ctx->selection->list, sn, lln) {
      if(vertex_kind_get(sn->v) != VERTEX_KIND_FSENT) {
        continue;
      }

      fse = containerof(sn->v, typeof(*fse), vertex);
      describe_fsent(fse, sn->rank, builder_writer);
    }
  }

  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(ctx->chan, msg);
}

static void handler_fstree_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  fstree_describe(builder_writer);

  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(ctx->chan, msg);
}

static void handler_buildplan_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  buildplan_describe(builder_writer);

  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(ctx->chan, msg);
}

static void handler_filesystem_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  filesystem_describe(builder_writer);

  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(ctx->chan, msg);
}

static void handler_metadata(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  size_t z;
  bamipc_message *msg = 0;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  metadata_collate(msg->text, sizeof(msg->text), &z);

  msg->size = z;
  channel_post(ctx->chan, msg);
}

static void handler_global_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  bamipc_message *msg = 0;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  msg->size = stats_global_collate(msg->text, sizeof(msg->text), reset);

  channel_post(ctx->chan, msg);
}

static void handler_config_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  narrator_growing growing;
  narrator * N;
  int x;
  config *cfgn;
  size_t z;
  size_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  char *dst;
  size_t sz;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = descriptor_bam_config_describe_result.id;
  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_config_describe_result.id);

  /* list each contributing config file */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);            /* size of the list blob */
  z += align(z, 2);
  list_len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);            /* items in the list blob */

  x = 0;
  llist_foreach(&config_list, cfgn, vertex.owner) {
    z += marshal_u32(dst + z, sz - z, descriptor_bam_config_describe_string.id);
    z += marshal_u16(dst + z, sz - z, cfgn->self_node_abspath_len);
    z += zcatw(dst + z, sz - z, cfgn->self_node_abspath, cfgn->self_node_abspath_len);
    x++;
  }

  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
  marshal_u16(dst + list_len_off, sz - list_len_off, x);

  msg->size = z;
  channel_post(ctx->chan, msg);

  N = narrator_growing_init(&growing);
  if(configuration_active) {
    configuration_say(configuration_active, N);
  }

  x = 0;
  while(x < growing.l)
  {
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;

    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);

    channel_post(ctx->chan, msg);
    x += msg->size;
  }

  narrator_growing_destroy(&growing);
}

static void handler_vars_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  module *mod;
  narrator_growing growing;
  narrator * N;
  int x;
  struct var *var;
  moria_foreach_state st;
  moria_edge *edge;
  size_t z;
  size_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  size_t size_off;
  char *dst;
  size_t sz;

  if((mod = ctx->mod) == 0) {
    return;
  }

  N = narrator_growing_init(&growing);

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = descriptor_bam_vars_describe_result.id;
  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_vars_describe_result.id);

  /* directory abspath */
  z += marshal_u16(dst + z, sz - z, 0);
  size_off = z - 2;
  z += fsent_absolute_path_cat(dst + z, sz - z, mod->dir_node);
  marshal_u16(dst + size_off, sz - size_off, z - size_off - 2);

  /* module id */
  z += marshal_u16(dst + z, sz - z, mod->id_len);
  z += zcatw(dst + z, sz - z, mod->id, mod->id_len);

  /* module abspath */
  z += marshal_u16(dst + z, sz - z, mod->dir_node_abspath_len);
  z += zcatw(dst + z, sz - z, mod->dir_node_abspath, mod->dir_node_abspath_len);

  /* list each contributing var.bam file */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);            /* size of the list blob */
  z += align(z, 2);
  list_len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);            /* items in the list blob */

  x = 0;
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_VAR_MOD) {
      continue;
    }

    var = containerof(edge->B, typeof(*var), vertex);

    z += marshal_u32(dst + z, sz - z, descriptor_bam_vars_describe_string.id);
    z += marshal_u16(dst + z, sz - z, var->self_node_abspath_len);
    z += zcatw(dst + z, sz - z, var->self_node_abspath, var->self_node_abspath_len);
    x++;
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
  marshal_u16(dst + list_len_off, sz - list_len_off, x);

  msg->size = z;
  channel_post(ctx->chan, msg);

  /* finally, the effective vars text */
  growing.l = 0;
  bacon_value_say(mod->vars, N);

  x = 0;
  while(x < growing.l)
  {
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;
    msg->attrs = 0;
    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);
    channel_post(ctx->chan, msg);
    x += msg->size;
  }

  narrator_growing_destroy(&growing);
}

static void handler_envs_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  module *mod;
  int x;

  if((mod = ctx->mod) == 0) {
    return;
  }
  if(!mod->exec) {
    return;
  }

  for(x = 0; x < mod->exec->envs_size; x++) {
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;
    msg->size = MIN(strlen(mod->exec->envs[x]), sizeof(msg->text));
    memcpy(msg->text, mod->exec->envs[x], msg->size);
    channel_post(ctx->chan, msg);
  }
}

static void handler_rma_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  narrator_growing growing;
  bacon_writer writer, *builder_writer;
  rule_module_association *rma;
  uint32_t off;
  selected *sn;
  fsent *shadow;
  moria_foreach_state st;
  moria_edge *e;

  narrator_growing_init(&growing);

  llist_foreach(&ctx->selection->list, sn, lln) {
    if(vertex_type_get(sn->v) != VERTEX_SHADOW_RULE) {
      continue;
    }

    shadow = containerof(sn->v, typeof(*shadow), vertex);
    rma = shadow->self_rma;

    narrator_reset(&growing.base);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &growing.base);

    { SET
      { MAPPING
        STRING("rma")
        { SET
          vertex_describe(&rma->vertex, builder_writer);
        }
      }

      { MAPPING STRING("target_hits") UINT(rma->stats.target_hits) }
      { MAPPING STRING("attachment_hits") UINT(rma->stats.attachment_hits) }
      { MAPPING STRING("search_hits") UINT(rma->stats.search_hits) }
      { MAPPING STRING("generate_hits") UINT(rma->stats.generate_hits) }
      { MAPPING STRING("formula_hits") UINT(rma->stats.formula_hits) }

      /* bindings are created by lookup/attachment rules */
      { MAPPING
        STRING("bindings")
        { LIST
          moria_foreach_edge_up(&st, &rma->vertex, e) {
            switch(edge_type_get(e)) {
              case EDGE_RMA_TGT:
                break;
              default:
                continue;
            }
            vertex_describe(e->A, builder_writer);
          }
        }
      }

      /* bpes are created by search/generate rules */
      { MAPPING
        STRING("bpes")
        { LIST
          moria_foreach_edge_up(&st, &rma->vertex, e) {
            switch(edge_type_get(e)) {
              case EDGE_RMA_BPE:
                break;
              default:
                continue;
            }
            vertex_describe(e->A, builder_writer);
          }
        }
      }
    }

    bacon_writer_destroy(&writer);

    /* just stream off the text */
    for(off = 0; off < growing.l;)
    {
      msg = channel_produce(ctx->chan);
      msg->type = BAMIPC_MSG_RESULT;
      msg->size = MIN(sizeof(msg->text), growing.l - off);
      memcpy(msg->text, growing.s + off, msg->size);
      channel_post(ctx->chan, msg);
      off += msg->size;
    }
  }

  narrator_growing_destroy(&growing);
}

static void handler_rules_describe(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  narrator_fixed fixed;
  narrator * N;
  rule_module_association *rma;
  size_t z;
  size_t sz;
  char *dst;
  uint16_t off;
  moria_foreach_state st;
  moria_edge *edge;
  module *mod = 0;
  rules *rules;
  const char *path;
  uint16_t path_len;
  size_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  size_t size_off;
  int x;

  if((mod = ctx->mod) == 0) {
    return;
  }

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = descriptor_bam_rules_describe_result.id;
  dst = msg->text;
  sz = sizeof(msg->text);
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_rules_describe_result.id);

  /* directory abspath */
  z += marshal_u16(dst + z, sz - z, 0);
  size_off = z - 2;
  z += fsent_absolute_path_cat(dst + z, sz - z, mod->dir_node);
  marshal_u16(dst + size_off, sz - size_off, z - size_off - 2);

  /* module id */
  z += marshal_u16(dst + z, sz - z, mod->id_len);
  z += zcatw(dst + z, sz - z, mod->id, mod->id_len);

  /* module abspath */
  z += marshal_u16(dst + z, sz - z, mod->dir_node_abspath_len);
  z += zcatw(dst + z, sz - z, mod->dir_node_abspath, mod->dir_node_abspath_len);

  /* list each contributing file */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);            /* size of the list blob */
  z += align(z, 2);
  list_len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);            /* items in the list blob */

  /* describe the module.bam file */
  x = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_rules_describe_string.id);
  z += marshal_u16(dst + z, sz - z, mod->self_node_abspath_len);
  z += zcatw(dst + z, sz - z, mod->self_node_abspath, mod->self_node_abspath_len);
  x++;

  /* describe each connected rules.bam file */
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_RULES_MOD) {
      continue;
    }

    rules = containerof(edge->B, typeof(*rules), vertex);
    z += marshal_u32(dst + z, sz - z, descriptor_bam_rules_describe_string.id);
    z += marshal_u16(dst + z, sz - z, rules->self_node_abspath_len);
    z += zcatw(dst + z, sz - z, rules->self_node_abspath, rules->self_node_abspath_len);
    x++;
  }
  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
  marshal_u16(dst + list_len_off, sz - list_len_off, x);

  /* post */
  msg->size = z;
  channel_post(ctx->chan, msg);

  // now describe each connected rma
  moria_foreach_edge_down(&st, &mod->vertex, edge) {
    if(edge_type_get(edge) != EDGE_RMA_MOD) {
      continue;
    }

    rma = containerof(edge->B, typeof(*rma), vertex);

    /* rule stats */
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;
    msg->attrs = descriptor_bam_rule_stats.id;
    msg->size = rule_module_association_stats_collate(msg->text, sizeof(msg->text), false, rma);
    channel_post(ctx->chan, msg);

    /* rule info */
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;
    msg->attrs = descriptor_bam_rule_info.id;
    dst = msg->text;
    sz = sizeof(msg->text);
    z = 0;

    z += marshal_u32(dst + z, sz - z, descriptor_bam_rule_info.id);

    /* rule id */
    z += marshal_u16(dst + z, sz - z, rma->rule->id_len);
    z += zcatw(dst + z, sz - z, rma->rule->id, rma->rule->id_len);

    /* rule text */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    N = narrator_fixed_init(&fixed, dst + z, sz - z);
    rule_say(rma->rule, N);
    z += fixed.l;
    marshal_u16(dst + off, sz - off, fixed.l);

    /* file location */
    if(rma->rules) {
      path = rma->rules->self_node_abspath;
      path_len = rma->rules->self_node_abspath_len;
    } else {
      path = rma->mod->self_node_abspath;
      path_len = rma->mod->self_node_abspath_len;
    }

    z += marshal_u16(dst + z, sz - z, path_len);
    memcpy(dst + z, path, path_len);
    z += path_len;

    /* location numbers */
    z += marshal_u16(dst + z, sz - z, rma->rule->loc.f_lin);
    z += marshal_u16(dst + z, sz - z, rma->rule->loc.f_col);
    z += marshal_u16(dst + z, sz - z, rma->rule->loc.l_lin);
    z += marshal_u16(dst + z, sz - z, rma->rule->loc.l_col);

    msg->size = z;
    channel_post(ctx->chan, msg);
  }
}

static void handler_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  bamipc_message *msg;
  selected *sn;
  moria_vertex *v;
  fsent *fse = 0;
  module *mod;
  formula *fml;
  moria_vertex *special;
  struct var *var;
  config *config;
  struct rules *rules;
  rule_module_association *rma;

  sn = llist_first(&ctx->selection->list, typeof(*sn), lln);
  special = 0;
  while(1)
  {
    if(special) {
      v = special;
      special = 0;
    } else if(sn) {
      v = sn->v;
      sn = llist_next(&ctx->selection->list, sn, lln);
    } else {
      break;
    }

    fse = 0;
    if(vertex_kind_get(v) == VERTEX_KIND_FSENT) {
      fse = containerof(v, typeof(*fse), vertex);
    }

    switch(vertex_type_get(v)) {
      /* vertex types which dont have stats */
      case VERTEX_SHADOW_DIR:
      case VERTEX_SHADOW_LINK:
        continue;
      case VERTEX_SHADOW_MODULE:
        special = &fse->mod->vertex;
        continue;
      case VERTEX_SHADOW_RULE:
        special = &fse->self_rma->vertex;
        continue;
      /* vertex types with an associated special */
      case VERTEX_MODULE_DIR:
        special = &fse->mod->vertex;
        break;
      case VERTEX_MODULE_FILE:
        special = &fse->self_mod->vertex;
        break;
      case VERTEX_FORMULA_FILE:
        special = &fse->self_fml->vertex;
        break;
      case VERTEX_VAR_FILE:
        special = &fse->self_var->vertex;
        break;
      case VERTEX_CONFIG_FILE:
        special = &fse->self_config->vertex;
        break;
      case VERTEX_RULES_FILE:
        special = &fse->self_rules->vertex;
        break;
      default:
    }

    if(cmd->stats.special_only && vertex_kind_get(v) != VERTEX_KIND_SPECIAL) {
      continue;
    }
    if(cmd->stats.no_special && vertex_kind_get(v) == VERTEX_KIND_SPECIAL) {
      continue;
    }

    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;
    if(v) {
      switch(vertex_type_get(v)) {
        case VERTEX_SHADOW_DIR:
          break;
        case VERTEX_MODULE:
          mod = containerof(v, typeof(*mod), vertex);
          msg->size = module_stats_collate(msg->text, sizeof(msg->text), reset, mod);
          break;
        /* fsents */
        case VERTEX_FILE:
        case VERTEX_MODULE_FILE:
        case VERTEX_FORMULA_FILE:
        case VERTEX_VAR_FILE:
        case VERTEX_CONFIG_FILE:
        case VERTEX_RULES_FILE:
          msg->size = vertex_stats_collate(msg->text, sizeof(msg->text), reset, &fse->vertex);
          break;
        case VERTEX_DIR:
        case VERTEX_MODULE_DIR:
          msg->size = fsent_dir_stats_collate(msg->text, sizeof(msg->text), reset, fse);
          break;
        /* meta */
        case VERTEX_RMA:
          rma = containerof(v, typeof(*rma), vertex);
          msg->size = rule_module_association_stats_collate(msg->text, sizeof(msg->text), reset, rma);
          break;
        /* special */
        case VERTEX_FORMULA:
          fml = containerof(v, typeof(*fml), vertex);
          msg->size = formula_stats_collate(msg->text, sizeof(msg->text), reset, fml);
          break;
        case VERTEX_VAR:
          var = containerof(v, typeof(*var), vertex);
          msg->size = var_stats_collate(msg->text, sizeof(msg->text), reset, var);
          break;
        case VERTEX_CONFIG:
          config = containerof(v, typeof(*config), vertex);
          msg->size = config_stats_collate(msg->text, sizeof(msg->text), reset, config);
          break;
        case VERTEX_RULES:
          rules = containerof(v, typeof(*rules), vertex);
          msg->size = rules_stats_collate(msg->text, sizeof(msg->text), reset, rules);
          break;
        default:
          vertex_say(v, g_narrator_stdout);
          printf("\n");
          RTA;
      }
    }
    channel_post(ctx->chan, msg);
  }
}

static void handler_goals(handler_context * restrict ctx, command * restrict cmd)
{
  selector *direct = 0;
  selector *transitive = 0;

  direct = cmd->goals.target_direct;
  cmd->goals.target_direct = 0;

  transitive = cmd->goals.target_transitive;
  cmd->goals.target_transitive = 0;

  goals_set(ctx->chan->msgid, cmd->goals.reconcile, cmd->goals.build, cmd->goals.script, direct, transitive);
  direct = 0;
  transitive = 0;

  selector_free(direct);
  selector_free(transitive);
}

static void handler_destroy(handler_context * restrict ctx)
{
  selector_context_destroy(&ctx->sel_ctx);
  graph_invalidation_end(&ctx->invalidation);
  request_parser_free(ctx->request_parser);
}

//
// public
//

void handler_process_command(handler_context * restrict ctx, command * restrict cmd)
{
  switch(cmd->type) {
    case COMMAND_CONTEXT:
      handler_cmd_context(ctx, cmd);
      break;
    case COMMAND_SELECT:
      handler_select(ctx, cmd);
      break;
    case COMMAND_RESET_SELECTION:
      handler_reset_selection(ctx, cmd);
      break;
    case COMMAND_LIST:
      handler_list(ctx, cmd);
      break;
    case COMMAND_INVALIDATE:
      handler_invalidate(ctx, cmd);
      break;
    case COMMAND_GLOBAL_INVALIDATE:
      handler_global_invalidate(ctx, cmd);
      break;
    case COMMAND_DESCRIBE:
      handler_describe(ctx, cmd);
      break;
    case COMMAND_FSTREE_DESCRIBE:
      handler_fstree_describe(ctx, cmd);
      break;
    case COMMAND_BUILDPLAN_DESCRIBE:
      handler_buildplan_describe(ctx, cmd);
      break;
    case COMMAND_FILESYSTEM_DESCRIBE:
      handler_filesystem_describe(ctx, cmd);
      break;
    case COMMAND_GOALS:
      handler_goals(ctx, cmd);
      break;
    case COMMAND_GLOBAL_STATS_READ:
      handler_global_stats(ctx, cmd, false);
      break;
    case COMMAND_GLOBAL_STATS_RESET:
      handler_global_stats(ctx, cmd, true);
      break;
    case COMMAND_STATS_READ:
      handler_stats(ctx, cmd, false);
      break;
    case COMMAND_STATS_RESET:
      handler_stats(ctx, cmd, true);
      break;
    case COMMAND_CONFIG_DESCRIBE:
      handler_config_describe(ctx, cmd);
      break;
    case COMMAND_VARS_DESCRIBE:
      handler_vars_describe(ctx, cmd);
      break;
    case COMMAND_ENVS_DESCRIBE:
      handler_envs_describe(ctx, cmd);
      break;
    case COMMAND_RMA_DESCRIBE:
      handler_rma_describe(ctx, cmd);
      break;
    case COMMAND_RULES_DESCRIBE:
      handler_rules_describe(ctx, cmd);
      break;
    case COMMAND_METADATA:
      handler_metadata(ctx, cmd, true);
      break;
    case COMMAND_RECONCILE:
    case COMMAND_RUN:
    case COMMAND_AUTORUN:
      RTA;
  }
}

void handler_setup()
{
}

void handler_cleanup()
{
  handler_context *ctx;
  llist *T;

  llist_foreach_safe(&context_freelist, ctx, lln, T) {
    handler_destroy(ctx);
    free(ctx);
  }
}

void handler_alloc(handler_context ** restrict rv)
{
  handler_context * ctx;

  if((ctx = llist_shift(&context_freelist, typeof(*ctx), lln)) == 0)
  {
    xmalloc(&ctx, sizeof(*ctx));
    llist_init(&ctx->lln);
    request_parser_create(&ctx->request_parser);
    selector_context_init(&ctx->sel_ctx);
  }
  else
  {
    memset(&ctx->channel_state, 0, sizeof(*ctx) - offsetof(typeof(*ctx), channel_state));
  }

  *rv = ctx;
}

void handler_release(handler_context * restrict ctx)
{
  if(ctx) {
    llist_append(&context_freelist, &ctx->lln);
  }
}

void handler_reset(handler_context * restrict ctx)
{
  ctx->selection = 0;
  ctx->running = false;
  ctx->mod = 0;
}
