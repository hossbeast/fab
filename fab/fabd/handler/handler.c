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

#include "marshal.h"
#include "zbuffer.h"

struct trylock handler_build_lock;

static llist context_freelist = LLIST_INITIALIZER(context_freelist);
rcu_list g_handlers = RCU_LIST_INITIALIZER(g_handlers);

//
// handlers
//

static xapi handler_select(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.mod = g_project_root->mod;
  ctx->sel_ctx.chan = ctx->chan;

  fatal(selector_exec, cmd->selector, &ctx->sel_ctx, SELECTION_ITERATION_TYPE_ORDER);

  // return the active selection
  ctx->selection = ctx->sel_ctx.selection;

  finally : coda;
}

static xapi handler_reset_selection(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->selection = 0;

  finally : coda;
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

static xapi handler_list(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

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

  finally : coda;
}

static xapi handler_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  selected *sn;
  fsent *n;

  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);
    fatal(fsent_invalidate, n, &ctx->invalidation);
  }

  finally : coda;
}

static xapi handler_global_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  handler_context *handler;
  fabipc_message *msg;

  fsent_valid_epoch++;
  ctx->invalidation.any = true;

  if(events_would(FABIPC_EVENT_GLOBAL_INVALIDATE, &handler, &msg))
  {
    events_publish(handler, msg);
  }

  finally : coda;
}

static xapi handler_describe(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

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

  finally : coda;
}

static xapi handler_metadata(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  enter;

  size_t z;
  fabipc_message *msg = 0;

  msg = channel_produce(ctx->chan);
  msg->type = FABIPC_MSG_RESULT;

  fatal(metadata_collate, msg->text, sizeof(msg->text), &z);

  msg->size = z;
  channel_post(ctx->chan, msg);

  finally : coda;
}

static xapi handler_global_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  enter;

  fabipc_message *msg = 0;
  size_t z;

  msg = channel_produce(ctx->chan);
  msg->type = FABIPC_MSG_RESULT;

  z = 0;
  fatal(stats_global_collate, msg->text, sizeof(msg->text), reset, &z);
  msg->size = z;

  channel_post(ctx->chan, msg);

  finally : coda;
}

static xapi handler_config_read(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  fabipc_message *msg;
  narrator_growing growing;
  narrator * N;
  int x;

  N = narrator_growing_init(&growing);
  fatal(config_active_say, N);

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

finally:
  fatal(narrator_growing_destroy, &growing);
coda;
}

static xapi handler_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  enter;

  fabipc_message *msg;
  selected *sn;
  fsent *n;
  size_t z;

  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);

    msg = channel_produce(ctx->chan);
    msg->type = FABIPC_MSG_RESULT;

    z = 0;
    fatal(stats_node_collate, msg->text, sizeof(msg->text), n, reset, &z);
    msg->size = z;

    channel_post(ctx->chan, msg);
  }

  finally : coda;
}

static xapi handler_goals(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  selector *direct = 0;
  selector *transitive = 0;

  direct = cmd->goals.target_direct;
  cmd->goals.target_direct = 0;

  transitive = cmd->goals.target_transitive;
  cmd->goals.target_transitive = 0;

  fatal(goals_set, ctx->chan->msgid, cmd->goals.build, cmd->goals.script, direct, transitive);
  direct = 0;
  transitive = 0;

finally:
  selector_free(direct);
  selector_free(transitive);
coda;
}

static xapi handler_destroy(handler_context * restrict ctx)
{
  enter;

  fatal(selector_context_xdestroy, &ctx->sel_ctx);
  graph_invalidation_end(&ctx->invalidation);
  fatal(request_parser_xfree, ctx->request_parser);

  finally : coda;
}

//
// public
//

xapi handler_process_command(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  if(cmd->type == COMMAND_SELECT)
  {
    fatal(handler_select, ctx, cmd);
  }
  else if(cmd->type == COMMAND_RESET_SELECTION)
  {
    fatal(handler_reset_selection, ctx, cmd);
  }
  else if(cmd->type == COMMAND_LIST)
  {
    fatal(handler_list, ctx, cmd);
  }
  else if(cmd->type == COMMAND_INVALIDATE)
  {
    fatal(handler_invalidate, ctx, cmd);
  }
  else if(cmd->type == COMMAND_GLOBAL_INVALIDATE)
  {
    fatal(handler_global_invalidate, ctx, cmd);
  }
  else if(cmd->type == COMMAND_DESCRIBE)
  {
    fatal(handler_describe, ctx, cmd);
  }
  else if(cmd->type == COMMAND_GOALS)
  {
    fatal(handler_goals, ctx, cmd);
  }
  else if(cmd->type == COMMAND_GLOBAL_STATS_READ)
  {
    fatal(handler_global_stats, ctx, cmd, false);
  }
  else if(cmd->type == COMMAND_GLOBAL_STATS_RESET)
  {
    fatal(handler_global_stats, ctx, cmd, true);
  }
  else if(cmd->type == COMMAND_STATS_READ)
  {
    fatal(handler_stats, ctx, cmd, false);
  }
  else if(cmd->type == COMMAND_STATS_RESET)
  {
    fatal(handler_stats, ctx, cmd, true);
  }
  else if(cmd->type == COMMAND_CONFIG_READ)
  {
    fatal(handler_config_read, ctx, cmd);
  }
  else if(cmd->type == COMMAND_METADATA)
  {
    fatal(handler_metadata, ctx, cmd, true);
  }
  else if(cmd->type == COMMAND_BOOTSTRAP) { }
  else if(cmd->type == COMMAND_RECONCILE) { }
  else if(cmd->type == COMMAND_RUN)
  {
    goals_autorun = false;
  }
  else if(cmd->type == COMMAND_AUTORUN)
  {
    goals_autorun = true;
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}

xapi handler_setup()
{
  enter;

  finally : coda;
}

xapi handler_cleanup()
{
  enter;

  handler_context *ctx;
  llist *T;

  llist_foreach_safe(&context_freelist, ctx, lln, T) {
    fatal(handler_destroy, ctx);
    wfree(ctx);
  }

  finally : coda;
}

xapi handler_alloc(handler_context ** restrict rv)
{
  enter;

  handler_context * ctx;

  if((ctx = llist_shift(&context_freelist, typeof(*ctx), lln)) == 0)
  {
    fatal(xmalloc, &ctx, sizeof(*ctx));
    llist_init_node(&ctx->lln);
    fatal(request_parser_create, &ctx->request_parser);
  }
  else
  {
    memset(&ctx->channel_state, 0, sizeof(*ctx) - offsetof(typeof(*ctx), channel_state));
  }

  *rv = ctx;

  finally : coda;
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
