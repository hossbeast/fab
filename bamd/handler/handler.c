/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "moria/foreach.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/builder.h"
#include "common/hash.h"

#include "handler.h"
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
#include "buildplan_entity.h"
#include "channel.h"
#include "run_thread.h"
#include "rule_module_association.h"
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

#include "marshal.h"
#include "zcat.h"

struct trylock handler_build_lock;

static llist context_freelist = LLIST_INITIALIZER(context_freelist);
rcu_list g_handlers = RCU_LIST_INITIALIZER(g_handlers);

//
// handlers
//

static void handler_context_cmd(handler_context * restrict ctx, command * restrict cmd)
{
  moria_vertex *v;
  fsent *fse;
  bamipc_message *msg;

  if((v = moria_vertex_downw(&g_shadow_modules->vertex, cmd->context.module_id, cmd->context.module_id_len)) == 0) {
    msg = channel_produce(ctx->chan);
    msg->code = -1;
    msg->type = BAMIPC_MSG_RESPONSE;
    msg->size = zcatf(msg->text, sizeof(msg->text), "module not found %.*s", cmd->context.module_id_len, cmd->context.module_id);
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
  ctx->sel_ctx.cwd = g_base;
  ctx->sel_ctx.chan = ctx->chan;
  ctx->sel_ctx.iteration_type = SELECTION_ITERATION_TYPE_ORDER;

  selector_exec(cmd->selector, &ctx->sel_ctx);

  // return the active selection
  ctx->selection = ctx->sel_ctx.selection;
}

static void handler_reset_selection(handler_context * restrict ctx, command * restrict cmd)
{
  ctx->selection = 0;
}

static void handler_list(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  selected *sn;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { SET
    llist_foreach(&ctx->selection->list, sn, lln) {
      vertex_identity_describe(sn->v, builder_writer);
    }
  }

  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(ctx->chan, msg);
}

static void handler_identify(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  selected *sn;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = channel_produce(ctx->chan);
  msg->type = BAMIPC_MSG_RESULT;

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { SET
    llist_foreach(&ctx->selection->list, sn, lln) {
      vertex_id_describe(sn->v, builder_writer);
    }
  }

  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(ctx->chan, msg);
}

static void handler_tell(handler_context * restrict ctx, command * restrict cmd)
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

  vertex_tell(v, ctx->chan);
}

static void handler_invalidate(handler_context * restrict ctx, command * restrict cmd)
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

static void handler_invalidate_primary(handler_context * restrict ctx, command * restrict cmd)
{
  events_context ectx;
  event_bacon *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;
  uint32_t count;
  graph_invalidation_context invalidation = { };

  graph_invalidation_begin(&invalidation);
  count = fsent_invalidate_primary(&invalidation);
  graph_invalidation_end(&invalidation);

  if(count) {
    ctx->invalidated_any = true;
  }

  if((msg = events_bacon_would(&ectx, BAMIPC_EVENT_GLOBAL_INVALIDATION)))
  {
    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    { MAPPING
      STRING("invalidate-primary")
      { SET
        { MAPPING
          STRING("count")
          UINT(count)
        }
      }
    }

    bacon_writer_close(&writer);
    msg->id = 0;
    msg->size = fixed.l;
    events_publish(&ectx);
  }
}

static void handler_eventsub(handler_context * restrict ctx, command * restrict cmd)
{
  uint32_t delta;

  delta = cmd->mask & ~ctx->event_mask;
  ctx->event_mask = cmd->mask;

  /* fire gratuitous events for new subscriptions */
  if(delta & BAMIPC_EVENT_GOALS_SET) {
    goals_set_event();
  }

  if(delta & BAMIPC_EVENT_AUTORUN_SET) {
    autorun_set_event();
  }

  if(delta & BAMIPC_EVENT_SYSTEM_STATE_CHANGE) {
    system_state_event();
  }
}

static void handler_destroy(handler_context * restrict ctx)
{
  selector_context_destroy(&ctx->sel_ctx);
  request_parser_free(ctx->request_parser);
}

//
// public
//

void handler_process_command(handler_context * restrict ctx, command * restrict cmd)
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
    case COMMAND_INVALIDATE_PRIMARY:
      handler_invalidate_primary(ctx, cmd);
      break;
    case COMMAND_GLOBAL_STATS_READ:
    case COMMAND_GLOBAL_STATS_RESET:
    case COMMAND_STATS_READ:
      stats_handler(ctx, cmd, false);
      break;
    case COMMAND_STATS_RESET:
      stats_handler(ctx, cmd, true);
      break;
    case COMMAND_EVENTSUB:
      handler_eventsub(ctx, cmd);
      break;
    case COMMAND_AUTORUN:
      run_thread_autorun_set(cmd->autorun.enabled);
      break;
    case COMMAND_PLUGIN_READY:
      break;
    /* handled directly in handler_thread */
    case COMMAND_GOALS:
    case COMMAND_RECONCILE:
    case COMMAND_RUN:
    //default:
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
    memset(ctx, 0, sizeof(*ctx) - offsetof(typeof(*ctx), scratch_space));
  }

  llist_init(&ctx->events_txq);

  *rv = ctx;
}

void handler_release(handler_context * restrict ctx)
{
  RUNTIME_ASSERT(llist_empty(&ctx->events_txq));

  llist_append(&context_freelist, &ctx->lln);
}

void handler_reset(handler_context * restrict handler)
{
  handler->selection = 0;
  handler->running = false;
  handler->mod = 0;
  handler->invalidated_any = false;
}
