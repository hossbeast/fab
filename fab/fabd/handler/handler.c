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

#include "handler.h"
#include "buildplan.h"
#include "config.h"
#include "formula.h"
#include "logging.h"
#include "module.internal.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "path.h"
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

#include "macros.h"
#include "common/attrs.h"

int32_t handler_lock;
int32_t handler_build_lock;

static llist context_freelist = LLIST_INITIALIZER(context_freelist);
stack g_handlers = STACK_INITIALIZER(g_handlers);

//
// handlers
//

static xapi handler_select(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.base = g_project_root->mod->dir_node;
  ctx->sel_ctx.mod = g_project_root->mod;

  fatal(selector_exec, cmd->selector, &ctx->sel_ctx);

  // active selection
  ctx->selection = ctx->sel_ctx.selection;

  finally : coda;
}

static xapi handler_reset_selection(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->selection = 0;

  finally : coda;
}

static xapi handler_list(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  char space[512];
  node_property_context pctx;
  char pspace[512];
  selected_node *sn;
  size_t z;
  fabipc_message *msg;
  uint32_t tail;

  pctx.mod = g_project_root->mod;
  llist_foreach(&ctx->selection->list, sn, lln) {
    // render the node property to a string
    z = node_property_znload(space, sizeof(space), sn->n, cmd->property, &pctx);

    // encode as bacon string
    z = value_string_znloadw(pspace, sizeof(pspace), space, z);

    msg = handler_produce(ctx, &tail);
    memcpy(msg->text, pspace, z);
    msg->id = ctx->client_msg_id;
    msg->size = z;
    msg->type = FABIPC_MSG_RESULT;
    handler_post(ctx, tail);
  }

  finally : coda;
}

static xapi handler_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  selected_node *sn;

  llist_foreach(&ctx->selection->list, sn, lln) {
    fatal(node_invalidate, sn->n, &ctx->invalidation);
  }

  finally : coda;
}

static xapi handler_global_invalidate(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  handler_context *handler;
  fabipc_message *msg;
  uint32_t tail;

  node_valid_epoch++;
  ctx->invalidation.any = true;

  if(events_would(FABIPC_EVENT_GLOBAL_INVALIDATION, &handler, &msg, &tail))
  {
    events_publish(handler, msg, tail);
  }

  finally : coda;
}

static xapi handler_describe(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  char space[512];
  narrator_fixed nstor;
  narrator * N;
  uint32_t tail;

  char pspace[512];

  node_property_context pctx;
  selected_node *sn;
  size_t len;
  int x;
  value_writer writer;
  fabipc_message *msg;
  value_writer_init(&writer);

  pctx.mod = g_project_root->mod;
  llist_foreach(&ctx->selection->list, sn, lln) {
    N = narrator_fixed_init(&nstor, space, sizeof(space));
    fatal(value_writer_open, &writer, N);
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_bytes, &writer, sn->n->name->name, sn->n->name->namel);
    fatal(value_writer_push_set, &writer);
    for(x = 0; x < node_property_attrs->num; x++)
    {
      len = node_property_znload(pspace, sizeof(pspace), sn->n, node_property_attrs->members[x].value, &pctx);

      fatal(value_writer_push_mapping, &writer);
        fatal(value_writer_bytes, &writer, node_property_attrs->members[x].name, node_property_attrs->members[x].namel);
        fatal(value_writer_bytes, &writer, pspace, len);
      fatal(value_writer_pop_mapping, &writer);
    }
    fatal(value_writer_pop_set, &writer);
    fatal(value_writer_pop_mapping, &writer);
    fatal(value_writer_close, &writer);
    len = nstor.l;

    msg = handler_produce(ctx, &tail);
    msg->id = ctx->client_msg_id;
    msg->size = len;
    msg->type = FABIPC_MSG_RESULT;
    memcpy(msg->text, space, len);
    handler_post(ctx, tail);
  }

  finally : coda;
}

static xapi handler_goals(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  fatal(goals_set, ctx->client_msg_id, cmd->goals.build, cmd->goals.script, cmd->goals.target_direct, cmd->goals.target_transitive);

  finally : coda;
}

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
  uint32_t tail;

  msg = handler_produce(ctx, &tail);
  msg->id = ctx->client_msg_id;
  msg->size = text_len;
  msg->type = FABIPC_MSG_RESPONSE;
  msg->code = code;
  if(text_len) {
    memcpy(msg->text, text, text_len);
  }
  handler_post(ctx, tail);
}

void handler_request_complete(handler_context * restrict ctx, int code)
{
  handler_request_completes(ctx, code, 0, 0);
}

xapi handler_process_request(handler_context * restrict ctx, request * restrict req)
{
  enter;

  int x;
  bool staging;
  bool reconfiguring;
  command *cmd;

  // validation
  staging = false;
  reconfiguring = false;

  for(x = 0; x < req->commands->size; x++)
  {
    command * cmd = array_get(req->commands, x);
    if(cmd->type == COMMAND_STAGE_CONFIG)
    {
      if(reconfiguring)
        break;

      staging = true;
    }
    else if(cmd->type == COMMAND_RECONFIGURE)
    {
      if(reconfiguring || !staging)
        break;

      reconfiguring = true;
    }
  }

  if(x < req->commands->size)
    fail(REQUEST_ILLCONFIG);

  if(staging)
    fatal(config_begin_staging);

  for(x = 0; x < req->commands->size; x++)
  {
    cmd = array_get(req->commands, x);

    if(cmd->type == COMMAND_STAGE_CONFIG)
    {
      fatal(config_stage, &cmd->config);
    }
    else if(cmd->type == COMMAND_RECONFIGURE)
    {
      fatal(config_reconfigure);
    }
    else if(cmd->type == COMMAND_SELECT)
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

    /* only permitted as the final command in the request */
    else if(cmd->type == COMMAND_RUN)
    {
      fatal(handler_run, ctx);
    }
    else if(cmd->type == COMMAND_AUTORUN)
    {
      fatal(handler_autorun, ctx);
    }
    else
    {
      RUNTIME_ABORT();
    }
  }

  finally : coda;
}


fabipc_message * handler_produce(handler_context * restrict ctx, uint32_t * restrict tail)
{
  return fabipc_produce(
      ctx->chan->server_ring.pages
    , &ctx->chan->server_ring.head
    , &ctx->local_tail
    , tail
    , FABIPC_SERVER_RINGSIZE - 1
  );
}

void handler_post(handler_context * restrict ctx, uint32_t tail)
{
  fabipc_post(
      &ctx->chan->server_ring.tail
    , &ctx->local_tail
    , tail
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

  return msg;
}

void handler_consume(handler_context * restrict ctx)
{
  fabipc_consume(&ctx->chan->client_ring.head);
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
  ctx->chan = 0;
}
