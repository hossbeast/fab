/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "moria/foreach.h"
#include "bam/ipc.h"
#include "bam/pipe.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/builder.h"
#include "common/hash.h"

#include "request_handler.h"
#include "filesystem.h"
#include "system.h"
#include "configset.h"
#include "config.h"
#include "describe.h"
#include "plugin.h"
#include "events.h"
#include "goals.h"
#include "module.h"
#include "meta_edge.h"
#include "request.h"
#include "request_parser.h"
#include "stats.h"
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
#include "events.h"

#include "marshal.h"
#include "zcat.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

//
// handlers
//

static void handler_context_cmd(request_handler * restrict ctx, command * restrict cmd)
{
  moria_vertex *v;
  fsent *fse;
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  const char *code;

  if((v = moria_vertex_downw(&g_shadow_modules->vertex, cmd->context.module_id, cmd->context.module_id_len)) == 0) {
    msg = request_handler_tx(ctx);
    msg->code = BAMIPC_MSG_ENOENT;
    msg->type = BAMIPC_MSG_RESPONSE;

    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);
    { MAPPING
      STRING("error")
      { SET
        { MAPPING STRING("code")
          if((code = attrs32_name_byvalue(&bamipc_msg_code_attrs, msg->code))) {
            STRING(code)
          } else {
            UINT(msg->code)
          }
        }
        { MAPPING STRING("msg")
          LOADF("module not found %.*s", cmd->context.module_id_len, cmd->context.module_id)
        }
      }
    }
    bacon_writer_close(&writer);

    msg->len = fixed.l;
    request_handler_post(ctx, msg);
    return;
  }

  fse = containerof(v, typeof(*fse), vertex);
  ctx->mod = fse->self_mod;
}

static void handler_select(request_handler * restrict ctx, command * restrict cmd)
{
  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.mod = ctx->mod;
  ctx->sel_ctx.cwd = g_base;
//  ctx->sel_ctx.channel = ctx->channel;
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
  bamipc_message *msg;
  selected *sn;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  int x;

  bacon_writer_init(&writer);

  x = 0;
  llist_foreach(&ctx->selection->list, sn, lln) {
    if((x % 64) == 0) {
      if(x) {
        bacon_writer_destroy(&writer);
        msg->type = BAMIPC_MSG_RESULT;
        msg->len = fixed.l;
        request_handler_post(ctx, msg);
      }

      msg = request_handler_tx(ctx);
      narrator_fixed_init(&fixed, msg->text, msg->size);
      bacon_writer_init(&writer);
      builder_writer = bacon_writer_open(&writer, &fixed.base);
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

  msg = request_handler_tx(ctx);
  msg->type = BAMIPC_MSG_RESULT;

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
    case COMMAND_CONTEXT:
      handler_context_cmd(ctx, cmd);
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
    case COMMAND_GLOBAL_STATS_READ:
    case COMMAND_GLOBAL_STATS_RESET:
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
    //default:
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
    memset(handler, 0, sizeof(*handler) - offsetof(typeof(*handler), scratch_space));
  } else {
    xmalloc(&handler, sizeof(*handler));
    llist_init(&handler->lln);
    request_parser_create(&handler->request_parser);
    selector_context_init(&handler->sel_ctx);
  }

  //llist_init(&handler->events_txq);

  return handler;
}

void request_handler_release(request_handler * restrict handler)
{
  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &handler->lln);
  futexlock_release(&freelist_lock);
}

void request_handler_reset(request_handler * restrict handler)
{
  handler->selection = 0;
  handler->mod = 0;
  handler->invalidated_any = false;
  handler->request_error = false;
}

struct bamipc_message *request_handler_tx(request_handler * restrict handler)
{
  bamipc_message *msg;

  if(handler->channel) {
    msg = channel_produce(handler->channel, handler->request_msgid);
  } else {
    bamipc_tx(handler->pipe, 0, &msg);
  }

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
