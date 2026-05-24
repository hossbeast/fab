/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "common/attrs.h"
#include "moria/foreach.h"
#include "bam/ipc.h"
#include "bam/pipe.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/builder.h"

#include "request_handler.h"
#include "filesystem.h"
#include "system.h"
#include "configset.h"
#include "config.h"
#include "describe.h"
#include "plugin.h"
#include "events/events.h"
#include "goals.h"
#include "module.h"
#include "meta_edge.h"
#include "request.h"
#include "request_parser.h"
#include "stats.h"
#include "dependency.h"
#include "bpe.h"
#include "channel.h"
#include "run_thread.h"
#include "rma.h"
#include "vertex.h"
#include "dir_rma_edge.h"
#include "exec.h"
#include "varset.h"
#include "edge.h"
#include "formula.h"
#include "ruleset.h"
#include "fsent.h"
#include "shadow.h"
#include "buildplan.h"
#include "tree_list.h"
#include "fstree.h"
#include "module_system.h"

#include "marshal.h"
#include "zcat.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

//
// handlers
//

static void handler_context_module_cmd(request_handler * restrict ctx, command * restrict cmd)
{
  selected *sn;

  ctx->mod = 0;
  if(llist_count(&ctx->selection->list) != 1) {
    return;
  }
  sn = llist_first(&ctx->selection->list, typeof(*sn), lln);
  if(vertex_type_get(sn->v) != VERTEX_MODULE) {
    return;
  }
  ctx->mod = containerof(sn->v, typeof(*ctx->mod), vertex);
}

static void handler_select(request_handler * restrict ctx, command * restrict cmd)
{
  ctx->sel_ctx.fst = 0;
  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.mod = ctx->mod;
  ctx->sel_ctx.cwd = g_base;
  ctx->sel_ctx.handler = ctx;
  ctx->sel_ctx.iteration_type = SELECTION_ITERATION_TYPE_ORDER;

  selector_exec(cmd->selector, &ctx->sel_ctx);

  // return the active selection
  ctx->selection = ctx->sel_ctx.selection;
}

static void handler_reset_selection(request_handler * restrict ctx, command * restrict cmd)
{
  ctx->selection = 0;
}

static void handler_list(request_handler * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg = 0;
  selected *sn;
  bacon_writer writer, *builder_writer = 0;
  narrator_fixed fixed;
  int x;

  bacon_writer_init(&writer);

  x = 0;
  llist_foreach(&ctx->selection->list, sn, lln) {
    if((x % 64) == 0) {
      if(x) {
        bacon_writer_destroy(&writer);
        msg->len = fixed.l;
        request_handler_post(ctx, msg);
      }

      msg = request_handler_tx(ctx, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);
      narrator_fixed_init(&fixed, msg->text, msg->size);
      bacon_writer_init(&writer);
      writer.flat = true;
      builder_writer = bacon_writer_open(&writer, &fixed.base);
    }
    if(x) {
      bacon_writer_flat_newline(builder_writer);
    }
    vertex_identity_describe(sn->v, builder_writer);
    x++;
  }
  if(x) {
    msg->type = BAMIPC_MSG_RESULT;
    msg->len = fixed.l;
    request_handler_post(ctx, msg);
  }

  bacon_writer_destroy(&writer);
}

static void handler_identify(request_handler * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  selected *sn;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = request_handler_tx(ctx, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { SET
    llist_foreach(&ctx->selection->list, sn, lln) {
      vertex_id_describe(sn->v, builder_writer);
    }
  }

  bacon_writer_destroy(&writer);
  msg->len = fixed.l;
  request_handler_post(ctx, msg);
}

static void handler_tell(request_handler * restrict ctx, command * restrict cmd)
{
  selected *sn;
  moria_vertex *v;

  /* tell operates on a single vertex */
  v = 0;
  llist_foreach(&ctx->selection->list, sn, lln) {
    if(v) {
      return;
    }
    v = sn->v;
  }
  if(!v) {
    return;
  }

  vertex_tell(v, ctx);
}

static void handler_invalidate(request_handler * restrict ctx, command * restrict cmd)
{
  selected *sn;
  graph_invalidation_context invalidation = { };

  graph_invalidation_begin(&invalidation);

  llist_foreach(&ctx->selection->list, sn, lln) {
    vertex_invalidate(sn->v, &invalidation);
  }

  graph_invalidation_end(&invalidation);
  ctx->invalidated_any |= invalidation.any;
}

static void handler_modules_invalidate_primary(request_handler * restrict ctx, command * restrict cmd)
{
  graph_invalidation_context invalidation = { };

  graph_invalidation_begin(&invalidation);
  modules_invalidate_primary(&invalidation);
  graph_invalidation_end(&invalidation);

  ctx->invalidated_any = invalidation.any;
}

static void handler_destroy(request_handler * restrict ctx)
{
  selector_context_destroy(&ctx->sel_ctx);
  request_parser_free(ctx->request_parser);
}

//
// public
//

void request_handler_process_command(request_handler * restrict ctx, command * restrict cmd)
{
  switch(cmd->type) {
    case COMMAND_CONTEXT_MODULE:
      handler_context_module_cmd(ctx, cmd);
      break;
    case COMMAND_SELECT:
      handler_select(ctx, cmd);
      break;
    case COMMAND_RESET_SELECTION:
      handler_reset_selection(ctx, cmd);
      break;
    case COMMAND_TREE_LIST:
      tree_list_handler(ctx, cmd);
      break;
    case COMMAND_DESCRIBE:
      describe_handler(ctx, cmd);
      break;
    case COMMAND_IDENTIFY:
      handler_identify(ctx, cmd);
      break;
    case COMMAND_LIST:
      handler_list(ctx, cmd);
      break;
    case COMMAND_TELL:
      handler_tell(ctx, cmd);
      break;
    case COMMAND_INVALIDATE:
      handler_invalidate(ctx, cmd);
      break;
    case COMMAND_MODULES_INVALIDATE_PRIMARY:
      handler_modules_invalidate_primary(ctx, cmd);
      break;
    case COMMAND_STATS_READ:
      stats_handler(ctx, cmd, false);
      break;
    case COMMAND_STATS_RESET:
      stats_handler(ctx, cmd, true);
      break;
    case COMMAND_AUTORUN:
      run_thread_autorun_set(cmd->autorun.enabled);
      break;
    case COMMAND_PLUGIN_READY:
      break;
    /* handled directly in handler_thread */
    case COMMAND_EVENTSUB:
    case COMMAND_GOALS:
    case COMMAND_RECONCILE:
    case COMMAND_RUN:
      RTA;
  }
}

void request_handler_setup()
{
}

void request_handler_cleanup()
{
  request_handler *handler;

  while((handler = llist_shift(&freelist, typeof(*handler), lln))) {
    ASAN_CLEAN(handler);
    handler_destroy(handler);
    free(handler);
  }
}

request_handler *request_handler_alloc()
{
  request_handler *handler = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    handler = llist_shift(&freelist, typeof(*handler), lln);
    futexlock_release(&freelist_lock);
  }

  if(handler) {
    ASAN_CLEAN(handler);
    memset(handler, 0, sizeof(*handler) - offsetof(typeof(*handler), scratch_space));
  } else {
    xmalloc(&handler, sizeof(*handler));
    llist_init(&handler->lln);
    request_parser_create(&handler->request_parser);
    selector_context_init(&handler->sel_ctx);
  }

  return handler;
}

void request_handler_release(request_handler * restrict handler)
{
  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &handler->lln);
  ASAN_TAINT(handler);
  futexlock_release(&freelist_lock);
}

void request_handler_reset(request_handler * restrict handler)
{
  handler->selection = 0;
  handler->mod = 0;
  handler->invalidated_any = false;
  handler->request_error = false;
}

struct bamipc_message *request_handler_tx(request_handler * restrict handler, bamipc_msg_type type, uint32_t format, int32_t code)
{
  bamipc_message *msg;

  if(handler->channel) {
    msg = channel_produce(handler->channel, handler->request_msgid);
  } else {
    bamipc_tx(handler->pipe, 0, &msg);
  }

  msg->type = type;
  msg->format = format;
  msg->code = code;
  msg->attrs = 0;
  msg->len = 0;

  return msg;
}

void request_handler_post(request_handler * restrict handler, bamipc_message * restrict msg)
{
  if(msg->type == BAMIPC_MSG_RESULT && msg->code != 0) {
    handler->request_error = true;
  }

  if(handler->channel) {
    channel_post(handler->channel, msg);
  } else {
    bamipc_publish(handler->pipe, msg);
  }
}
