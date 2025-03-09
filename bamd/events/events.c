/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/events.h"
#include "bam/pipe.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xsignal.h"
#include "narrator.h"

#include "events/events.h"
#include "channel.h"
#include "channel_thread.h"
#include "rcu_list.h"
#include "request_handler.h"
#include "command.h"
#include "plugin.h"
#include "plugin_thread.h"
#include "goals.h"
#include "system.h"
#include "run_thread.h"

#include "common/attrs.h"
#include "params.h"

static void event_copy(bamipc_message * restrict msg, bamipc_event_type evtype, bamipc_message * restrict src)
{
  msg->flags = 0;
  msg->code = 0;
  msg->type = BAMIPC_MSG_EVENTS;
  msg->evtype = evtype;
  if(src) {
    msg->id = src->id;
    msg->code = src->code;
    if((msg->len = src->len)) {
      memcpy(msg->text, src->text, src->len);
    }
  }
}

static channel_thread *channel_would(bamipc_event_type evtype)
{
  channel_thread *channel;

  rcu_list_foreach(&channel_thread_list, channel, stk) {
    if(channel->event_mask & evtype) {
      return channel;
    }
  }

  return 0;
}

static plugin_thread *plugin_would(events_context * restrict ctx, bamipc_event_type evtype)
{
  plugin_thread *plugin;

  rcu_list_foreach(&plugin_thread_list, plugin, stk) {
    if(plugin->event_mask & evtype) {
      return plugin;
    }
  }

  return 0;
}

bamipc_message *events_bacon_would(events_context * restrict ctx, bamipc_event_type evtype)
{
  channel_thread *channel;
  plugin_thread *plugin;

  switch(evtype) {
    /* these events have a binary payload and therefore cant be used here */
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_REAP_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      RTA;
    default:
      break;
  }

  if((channel = channel_would(evtype))) {
    ctx->channel = channel;
    ctx->plugin = 0;
    ctx->msg = channel_produce(ctx->channel->channel, 0);
  } else if((plugin = plugin_would(ctx, evtype))) {
    ctx->channel = 0;
    ctx->plugin = plugin;
    bamipc_tx(ctx->plugin->pipe, 0, &ctx->msg);
  } else {
    return 0;
  }

  ctx->msg->flags = 0;
  ctx->msg->code = 0;
  ctx->msg->type = BAMIPC_MSG_EVENTS;
  ctx->msg->evtype = evtype;
  return ctx->msg;
}

bamipc_message *events_binary_would(events_context * restrict ctx, bamipc_event_type evtype)
{
  channel_thread *channel;

  switch(evtype) {
    /* these events have a binary payload */
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_REAP_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      break;
    default:
      RTA;
  }

  if(!(channel = channel_would(evtype))) {
    return 0;
  }
  /* plugins cannot subscribe to binary events */

  ctx->channel = channel;
  ctx->plugin = 0;
  ctx->msg = channel_produce(ctx->channel->channel, 0);
  ctx->msg->flags = 0;
  ctx->msg->code = 0;
  ctx->msg->type = BAMIPC_MSG_EVENTS;
  ctx->msg->evtype = evtype;
  return ctx->msg;
}

bool events_would(bamipc_event_type type)
{
  return channel_would(type) != NULL;
}

void event_describe(bamipc_message * restrict e, bacon_writer * restrict builder_writer)
{
  bool bacon;

  bacon = true;
  switch(e->evtype) {
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_REAP_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      bacon = false;
  }

  { SET
    { MAPPING STRING("id")
      UINT(e->id)
    }
    { MAPPING STRING("type")
      STRING(attrs32_name_byvalue(&bamipc_event_type_attrs, e->evtype))
    }
    { MAPPING STRING("code")
      UINT(e->code)
    }
    if(bacon) {
      bacon_writer_push_bytes(builder_writer);
      narrator_sayw(builder_writer->N, e->text, e->len);
      bacon_writer_pop_bytes(builder_writer);
    } else {
      MAPPING STRING("text")
      BYTES(e->text, e->len);
    }
  }
}

void event_show(bamipc_message * restrict e)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  event_describe(e, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void events_publish(events_context * restrict ctx)
{
  bamipc_message *msg;
  channel_thread *channel;
  plugin_thread *plugin;
  uint32_t evtype;

  evtype = ctx->msg->evtype;

  /* replicate to any _other_ subscribed clients */
  rcu_list_foreach(&channel_thread_list, channel, stk) {
    if(ctx->channel && ctx->channel == channel) {
      continue;
    }
    if(!(channel->event_mask & evtype)) {
      continue;
    }
    msg = channel_produce(channel->channel, 0);
    event_copy(msg, evtype, ctx->msg);
    channel_post(channel->channel, msg);
  }

  /* replicate to any _other_ subscribed plugins */
  rcu_list_foreach(&plugin_thread_list, plugin, stk) {
    if(ctx->plugin && ctx->plugin == plugin) {
      continue;
    }
    if(!(plugin->event_mask & evtype)) {
      continue;
    }
    bamipc_tx(plugin->pipe, 0, &msg);
    event_copy(msg, evtype, ctx->msg);
    bamipc_publish(plugin->pipe, msg);
    uxtgkill(g_params.pid, plugin->tid, SIGUSR1);
  }

  /* print system errors to the server log */
  if((ctx->msg->flags & EVENT_SYSTEM_ERROR)) {
    event_show(ctx->msg);
  }

  /* sink to self */
  if(ctx->channel) {
    channel_post(ctx->channel->channel, ctx->msg);
  } else if(ctx->plugin) {
    bamipc_publish(ctx->plugin->pipe, ctx->msg);
    uxtgkill(g_params.pid, ctx->plugin->tid, SIGUSR1);
  } else if((ctx->msg->flags & EVENT_SYSTEM_ERROR)) {
    /* system_error_event, and there are no handlers subscribed to this event */
  } else {
    RTA;
  }
}

void events_setup(void)
{
}

void events_cleanup(void)
{
}

void events_trigger(request_handler * restrict handler, uint32_t delta)
{
  /* fire gratuitous events on this pipe for new subscriptions */

  if(delta & BAMIPC_EVENT_GOALS_SET) {
    goals_set_event(handler);
  }

  if(delta & BAMIPC_EVENT_AUTORUN_SET) {
    autorun_set_event(handler);
  }

  if(delta & BAMIPC_EVENT_SYSTEM_STATE_CHANGE) {
    system_state_event(handler);
  }
}
