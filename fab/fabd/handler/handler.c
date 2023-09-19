/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include "common/attrs.h"
#include "fab/describe.desc.h"
#include "fab/ipc.h"
#include "fab/list.desc.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "valyria/array.h"
#include "xlinux/xstdlib.h"

#include "handler.h"
#include "config.internal.h"
#include "events.h"
#include "goals.h"
#include "metadata.h"
#include "module.internal.h"
#include "request.internal.h"
#include "request_parser.h"
#include "stats.h"
#include "variant.h"
#include "dependency.h"
#include "channel.h"
#include "run_thread.h"

#include "marshal.h"
#include "zbuffer.h"

struct trylock handler_build_lock;

static llist context_freelist = LLIST_INITIALIZER(context_freelist);
rcu_list g_handlers = RCU_LIST_INITIALIZER(g_handlers);

//
// handlers
//

static void handler_select(handler_context * restrict ctx, command * restrict cmd)
{
  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.mod = g_project_root->mod;
  ctx->sel_ctx.chan = ctx->chan;

  selector_exec(cmd->selector, &ctx->sel_ctx, SELECTION_ITERATION_TYPE_ORDER);

  // return the active selection
  ctx->selection = ctx->sel_ctx.selection;
}

static void handler_reset_selection(handler_context * restrict ctx, command * restrict cmd)
{
  ctx->selection = 0;
}

static fsent *selected_node(selected * restrict sn)
{
  module *mod;
  fsent *n;

  if((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_MODULE) {
    mod = containerof(sn->v, module, vertex);
    n = mod->dir_node;
  } else {
    RUNTIME_ASSERT((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT);
    n = containerof(sn->v, fsent, vertex);
  }

  return n;
}

static void handler_list(handler_context * restrict ctx, command * restrict cmd)
{
  selected *sn;
  fsent *n;
  size_t z;
  fabipc_message *msg;
  fab_fsent_state state;
  fab_fsent_type type;
  char *dst;
  size_t sz;
  uint16_t len;
  uint16_t off;

  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);
    msg = channel_produce(ctx->chan);
    msg->type = FABIPC_MSG_RESULT;

    dst = msg->text;
    sz = sizeof(msg->text);

    z = 0;
    z += marshal_u32(dst + z, sz - z, descriptor_fab_list_item.id);

    /* type */
    type = vertex_kind_remap(fsent_kind_get(n));
    z += marshal_int(dst + z, sz - z, &type, sizeof(type));

    /* state */
    state = vertex_state_remap(fsent_state_get(n));
    z += marshal_int(dst + z, sz - z, &state, sizeof(state));

    /* label */
    z += marshal_u16(dst + z, sz - z, n->name.namel);
    z += znloadw(dst + z, sz - z, n->name.name, n->name.namel);

    /* abspath */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = fsent_absolute_path_znload(dst + z, sz - z, n);
    z += len;
    marshal_u16(dst + off, sz - off, len);

    /* ref */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    if(fsent_filetype_get(n) == VERTEX_FILETYPE_LINK) {
      len = fsent_absolute_path_znload(dst + z, sz - z, containerof(n->vertex.ref, fsent, vertex));
      z += len;
      marshal_u16(dst + off, sz - off, len);
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
  fsent *n;

  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);
    fsent_invalidate(n, &ctx->invalidation);
  }
}

static void handler_global_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  channel *chan;
  fabipc_message *msg;

  fsent_valid_epoch++;
  ctx->invalidation.any = true;

  if(events_would(FABIPC_EVENT_GLOBAL_INVALIDATE, &chan, &msg)) {
    events_publish(chan, msg);
  }
}

static void handler_describe(handler_context * restrict ctx, command * restrict cmd)
{
  selected *sn;
  fabipc_message *msg;
  fab_fsent_state state;
  fab_fsent_type type;
  fsent *n;
  const filesystem *fs;
  char *dst;
  size_t sz;
  uint16_t len;
  uint16_t off;
  size_t z;

  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);
    msg = channel_produce(ctx->chan);
    msg->type = FABIPC_MSG_RESULT;

    dst = msg->text;
    sz = sizeof(msg->text);

    z = 0;
    z += marshal_u32(dst + z, sz - z, descriptor_fab_describe_item.id);

    /* distance */
    z += marshal_u16(dst + z, sz - z, sn->rank);

    /* type */
    type = vertex_kind_remap(fsent_kind_get(n));
    z += marshal_int(dst + z, sz - z, &type, sizeof(type));

    /* state */
    state = vertex_state_remap(fsent_state_get(n));
    z += marshal_int(dst + z, sz - z, &state, sizeof(state));

    /* label */
    z += marshal_u16(dst + z, sz - z, n->name.namel);
    z += znloadw(dst + z, sz - z, n->name.name, n->name.namel);

    /* abspath */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = fsent_absolute_path_znload(dst + z, sz - z, n);
    z += len;
    marshal_u16(dst + off, sz - off, len);

    /* ref */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    if(fsent_filetype_get(n) == VERTEX_FILETYPE_LINK) {
      len = fsent_absolute_path_znload(dst + z, sz - z, containerof(n->vertex.ref, fsent, vertex));
      z += len;
      marshal_u16(dst + off, sz - off, len);
    }

    /* variant */
    if(n->var) {
      z += marshal_u16(dst + z, sz - z, n->var->norm_len);
      z += znloadw(dst + z, sz - z, n->var->norm, n->var->norm_len);
    } else {
      z += marshal_u16(dst + z, sz - z, 0);
    }

    /* ext */
    z += marshal_u16(dst + z, sz - z, n->name.extl);
    z += znloadw(dst + z, sz - z, n->name.ext, n->name.extl);

    /* suffix */
    z += marshal_u16(dst + z, sz - z, n->name.suffixl);
    z += znloadw(dst + z, sz - z, n->name.suffix, n->name.suffixl);

    /* base */
    z += marshal_u16(dst + z, sz - z, n->name.basel);
    z += znloadw(dst + z, sz - z, n->name.base, n->name.basel);

    /* fsroot */
    fs = fsent_filesystem_get(n);
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = filesystem_absolute_path_znload(dst + z, sz - z, fs);
    z += len;
    marshal_u16(dst + off, sz - off, len);

    /* invalidation */
    off = z;
    z += marshal_u16(dst + z, sz - z, 0);
    len = znload_attrs16(dst + z, sz - z, invalidate_attrs, fs->invalidate);
    z += len;
    marshal_u16(dst + off, sz - off, len);

    if(n->dep)
    {
      RUNTIME_ASSERT(n->dep->fml);
      off = z;
      z += marshal_u16(dst + z, sz - z, 0);
      len = fsent_absolute_path_znload(dst + z, sz - z, n->dep->fml);
      z += len;
      marshal_u16(dst + off, sz - off, len);
    }
    else
    {
      z += marshal_u16(dst + z, sz - z, 0);
    }

    msg->size = z;
    channel_post(ctx->chan, msg);
  }
}

static void handler_metadata(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  size_t z;
  fabipc_message *msg = 0;

  msg = channel_produce(ctx->chan);
  msg->type = FABIPC_MSG_RESULT;

  metadata_collate(msg->text, sizeof(msg->text), &z);

  msg->size = z;
  channel_post(ctx->chan, msg);
}

static void handler_global_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  fabipc_message *msg = 0;
  size_t z;

  msg = channel_produce(ctx->chan);
  msg->type = FABIPC_MSG_RESULT;

  z = 0;
  stats_global_collate(msg->text, sizeof(msg->text), reset, &z);
  msg->size = z;

  channel_post(ctx->chan, msg);
}

static void handler_config_read(handler_context * restrict ctx, command * restrict cmd)
{
  fabipc_message *msg;
  narrator_growing growing;
  narrator * N;
  int x;

  N = narrator_growing_init(&growing);
  config_active_say(N);

  x = 0;
  while(x < growing.l)
  {
    msg = channel_produce(ctx->chan);
    msg->type = FABIPC_MSG_RESULT;

    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);

    channel_post(ctx->chan, msg);
    x += msg->size;
  }

  narrator_growing_destroy(&growing);
}

static void handler_vars_read(handler_context * restrict ctx, command * restrict cmd)
{
  fabipc_message *msg;
  narrator_growing growing;
  narrator * N;
  int x;

  N = narrator_growing_init(&growing);
  module_vars_say(g_project_root->mod, N);

  x = 0;
  while(x < growing.l)
  {
    msg = channel_produce(ctx->chan);
    msg->type = FABIPC_MSG_RESULT;

    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);

    channel_post(ctx->chan, msg);
    x += msg->size;
  }

  narrator_growing_destroy(&growing);
}

static void handler_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  fabipc_message *msg;
  selected *sn;
  fsent *n;
  size_t z;

  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);

    msg = channel_produce(ctx->chan);
    msg->type = FABIPC_MSG_RESULT;

    z = 0;
    stats_node_collate(msg->text, sizeof(msg->text), n, reset, &z);
    msg->size = z;

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
  selector_context_xdestroy(&ctx->sel_ctx);
  graph_invalidation_end(&ctx->invalidation);
  request_parser_xfree(ctx->request_parser);
}

//
// public
//

void handler_process_command(handler_context * restrict ctx, command * restrict cmd)
{
  if(cmd->type == COMMAND_SELECT)
  {
    handler_select(ctx, cmd);
  }
  else if(cmd->type == COMMAND_RESET_SELECTION)
  {
    handler_reset_selection(ctx, cmd);
  }
  else if(cmd->type == COMMAND_LIST)
  {
    handler_list(ctx, cmd);
  }
  else if(cmd->type == COMMAND_INVALIDATE)
  {
    handler_invalidate(ctx, cmd);
  }
  else if(cmd->type == COMMAND_GLOBAL_INVALIDATE)
  {
    handler_global_invalidate(ctx, cmd);
  }
  else if(cmd->type == COMMAND_DESCRIBE)
  {
    handler_describe(ctx, cmd);
  }
  else if(cmd->type == COMMAND_GOALS)
  {
    handler_goals(ctx, cmd);
  }
  else if(cmd->type == COMMAND_GLOBAL_STATS_READ)
  {
    handler_global_stats(ctx, cmd, false);
  }
  else if(cmd->type == COMMAND_GLOBAL_STATS_RESET)
  {
    handler_global_stats(ctx, cmd, true);
  }
  else if(cmd->type == COMMAND_STATS_READ)
  {
    handler_stats(ctx, cmd, false);
  }
  else if(cmd->type == COMMAND_STATS_RESET)
  {
    handler_stats(ctx, cmd, true);
  }
  else if(cmd->type == COMMAND_CONFIG_READ)
  {
    handler_config_read(ctx, cmd);
  }
  else if(cmd->type == COMMAND_VARS_READ)
  {
    handler_vars_read(ctx, cmd);
  }
  else if(cmd->type == COMMAND_METADATA)
  {
    handler_metadata(ctx, cmd, true);
  }
  else
  {
    RUNTIME_ABORT();
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
    wfree(ctx);
  }
}

void handler_alloc(handler_context ** restrict rv)
{
  handler_context * ctx;

  if((ctx = llist_shift(&context_freelist, typeof(*ctx), lln)) == 0)
  {
    xmalloc(&ctx, sizeof(*ctx));
    llist_init_node(&ctx->lln);
    request_parser_create(&ctx->request_parser);
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
    llist_append(&context_freelist, ctx, lln);
  }
}

void handler_reset(handler_context * restrict ctx)
{
  ctx->selection = 0;
}
