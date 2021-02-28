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

#include "xapi.h"

#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstring.h"

#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "valyria/dictionary.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "value.h"
#include "value/writer.h"

#include "fab/ipc.h"
#include "fab/events.h"
#include "fab/metadata.desc.h"
#include "fab/fsent.h"
#include "fab/list.desc.h"

#include "handler.h"
#include "buildplan.h"
#include "config.h"
#include "formula.h"
#include "logging.h"
#include "module.internal.h"
#include "fsent.h"
#include "notify_thread.h"
#include "params.h"
#include "args.h"
#include "config.internal.h"
#include "walker.h"
#include "request.internal.h"
#include "request_parser.h"
#include "request.internal.h"
#include "selector.h"
#include "selector.internal.h"
#include "REQUEST.errtab.h"
#include "goals.h"
#include "server_thread.h"
#include "node_operations.h"
#include "handler_thread.h"
#include "channel.h"
#include "events.h"
#include "handler_thread.h"
#include "stats.h"
#include "metadata.h"

#include "macros.h"
#include "common/attrs.h"
#include "zbuffer.h"
#include "threads.h"
#include "marshal.h"
#include "hexdump.h"

struct trylock handler_build_lock;

static llist context_freelist = LLIST_INITIALIZER(context_freelist);
stack g_handlers = STACK_INITIALIZER(g_handlers);

//
// handlers
//

static xapi handler_select(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->sel_ctx.bpe = 0;
//  ctx->sel_ctx.base = g_project_root->mod->dir_node;
  ctx->sel_ctx.mod = g_project_root->mod;

  fatal(selector_exec, cmd->selector, &ctx->sel_ctx, SELECTION_ITERATION_TYPE_ORDER);

  ctx->selection = ctx->sel_ctx.selection;  // return the active selection

  // return the error if any
  if(ctx->sel_ctx.errlen) {
    memcpy(ctx->err, ctx->sel_ctx.err, ctx->sel_ctx.errlen);
    ctx->errlen = ctx->sel_ctx.errlen;
  }

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
    msg = handler_produce(ctx);
    msg->id = ctx->client_msg_id;
    msg->type = FABIPC_MSG_RESULT;

//printf("list node %p %s\n", sn->n, sn->n->name.name);

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
    handler_post(ctx, msg);
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

  if(events_would(FABIPC_EVENT_GLOBAL_INVALIDATION, &handler, &msg))
  {
    events_publish(handler, msg);
  }

  finally : coda;
}

static xapi handler_describe(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  narrator_fixed nstor;
  narrator * N;

  char pspace[512];

  fsent_property_context pctx;
  selected *sn;
  size_t len;
  int x;
  value_writer writer;
  fabipc_message *msg;
  value_writer_init(&writer);
  fsent *n;

  pctx.mod = g_project_root->mod;
  llist_foreach(&ctx->selection->list, sn, lln) {
    n = selected_node(sn);

    msg = handler_produce(ctx);
    msg->id = ctx->client_msg_id;
    msg->type = FABIPC_MSG_RESULT;

    N = narrator_fixed_init(&nstor, msg->text, sizeof(msg->text));
    fatal(value_writer_open, &writer, N);
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_bytes, &writer, n->name.name, n->name.namel);
    fatal(value_writer_push_set, &writer);
    for(x = 0; x < fsent_property_attrs->num; x++)
    {
      len = fsent_property_znload(pspace, sizeof(pspace), n, fsent_property_attrs->members[x].value, &pctx);

      fatal(value_writer_push_mapping, &writer);
        fatal(value_writer_bytes, &writer, fsent_property_attrs->members[x].name, fsent_property_attrs->members[x].namel);
        fatal(value_writer_bytes, &writer, pspace, len);
      fatal(value_writer_pop_mapping, &writer);
    }
    fatal(value_writer_pop_set, &writer);
    fatal(value_writer_pop_mapping, &writer);
    fatal(value_writer_close, &writer);

    msg->size = nstor.l;
    handler_post(ctx, msg);
  }

  finally : coda;
}

static xapi handler_metadata(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  enter;

  size_t z;
  fabipc_message *msg = 0;

  msg = handler_produce(ctx);
  msg->id = ctx->client_msg_id;
  msg->type = FABIPC_MSG_RESULT;

  fatal(metadata_collate, msg->text, sizeof(msg->text), &z);

  msg->size = z;
  handler_post(ctx, msg);

  finally : coda;
}

static xapi handler_global_stats(handler_context * restrict ctx, command * restrict cmd, bool reset)
{
  enter;

  fabipc_message *msg = 0;
  size_t z;

  msg = handler_produce(ctx);
  msg->id = ctx->client_msg_id;
  msg->type = FABIPC_MSG_RESULT;

  z = 0;
  fatal(stats_global_collate, msg->text, sizeof(msg->text), reset, &z);
  msg->size = z;

  handler_post(ctx, msg);

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
    msg = handler_produce(ctx);
    msg->id = ctx->client_msg_id;
    msg->type = FABIPC_MSG_RESULT;

    msg->size = MIN(growing.l - x, sizeof(msg->text));
    memcpy(msg->text, growing.s + x, msg->size);

    handler_post(ctx, msg);
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

    msg = handler_produce(ctx);
    msg->id = ctx->client_msg_id;
    msg->type = FABIPC_MSG_RESULT;

    z = 0;
    fatal(stats_node_collate, msg->text, sizeof(msg->text), n, reset, &z);
    msg->size = z;

    handler_post(ctx, msg);
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

  fatal(goals_set, ctx->client_msg_id, cmd->goals.build, cmd->goals.script, direct, transitive);
  direct = 0;
  transitive = 0;

finally:
  selector_free(direct);
  selector_free(transitive);
coda;
}

#if 0
static xapi handler_autorun(handler_context * restrict ctx)
{
  enter;

  g_server_autorun = true;

  finally : coda;
}

static xapi handler_run(handler_context * restrict ctx)
{
  enter;

  g_server_autorun = false;

  finally : coda;
}
#endif

static xapi handler_destroy(handler_context * restrict ctx)
{
  enter;

  fatal(selector_context_xdestroy, &ctx->sel_ctx);
  graph_invalidation_end(&ctx->invalidation);
  fatal(request_parser_xfree, ctx->request_parser);
  fatal(rule_run_context_xdestroy, &ctx->rule_ctx);

  finally : coda;
}

//
// public
//

void handler_request_completes(handler_context * restrict ctx, int code, const char * restrict text, uint16_t text_len)
{
  fabipc_message *msg;

  msg = handler_produce(ctx);
  msg->id = ctx->client_msg_id;
  msg->size = text_len;
  msg->type = FABIPC_MSG_RESPONSE;
  msg->code = code;
  if(text_len) {
    memcpy(msg->text, text, text_len);
  }

//printf("%s:%d %u\n", __FUNCTION__, __LINE__, tail);
//printf("CHANNEL shmid %d @ %p\n", ctx->chan->shmid, ctx->chan);
//printf(" client-ring\n");
//printf("  head %u\n", ctx->chan->client_ring.head);
//printf("  tail %u\n", ctx->chan->client_ring.tail);
//printf(" server-ring\n");
//printf("  head %u\n", ctx->chan->server_ring.head);
//printf("  tail %u\n", ctx->chan->server_ring.tail);

  handler_post(ctx, msg);
}

void handler_request_complete(handler_context * restrict ctx, int code)
{
  handler_request_completes(ctx, code, 0, 0);
}

xapi handler_process_request(handler_context * restrict ctx, request * restrict req)
{
  enter;

  int x;
  command *cmd;

  for(x = 0; x < req->commands->size; x++)
  {
    cmd = array_get(req->commands, x);

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

//    /* only permitted as the final command in the request */
//    else if(cmd->type == COMMAND_RUN)
//    {
//      fatal(handler_run, ctx);
//    }
//    else if(cmd->type == COMMAND_AUTORUN)
//    {
//      fatal(handler_autorun, ctx);
//    }

    else
    {
      RUNTIME_ABORT();
    }

    if(ctx->errlen) {
      break;
    }
  }

  finally : coda;
}

fabipc_message * handler_produce(handler_context * restrict ctx)
{
  return fabipc_produce(
      ctx->chan->server_ring.pages
    , &ctx->chan->server_ring.head
    , &ctx->chan->server_ring.tail
    , FABIPC_SERVER_RINGSIZE - 1
  );
}

void handler_post(handler_context * restrict ctx, fabipc_message * restrict msg)
{
#if DEBUG || DEVEL
  RUNTIME_ASSERT(msg->type);
  if(msg->type == FABIPC_MSG_EVENTS) {
    RUNTIME_ASSERT(msg->evtype);
  }
#endif

  fabipc_post(
      msg
    , &ctx->chan->server_ring.waiters
  );
}

fabipc_message * handler_acquire(handler_context * restrict ctx)
{
  fabipc_message *msg;

  msg = fabipc_acquire(
      ctx->chan->client_ring.pages
    , &ctx->chan->client_ring.head
    , &ctx->chan->client_ring.tail
    , FABIPC_CLIENT_RINGSIZE - 1
  );

#if DEBUG || DEVEL
  if (msg) {
    RUNTIME_ASSERT(msg->type);
  }
#endif

  return msg;
}

void handler_consume(handler_context * restrict ctx, fabipc_message * restrict msg)
{
  fabipc_consume(
      ctx->chan->client_ring.pages
    , &ctx->chan->client_ring.head
    , msg
    , FABIPC_CLIENT_RINGSIZE - 1
  );
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
    fatal(rule_run_context_xinit, &ctx->rule_ctx);
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
  ctx->build_state = 0;
  ctx->errlen = 0;
}
