/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/events.h"
#include "bacon/writer.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xsignal.h"

#include "events.h"
#include "channel.h"
#include "handler_thread.h"
#include "rcu_list.h"
#include "handler.h"
#include "plugin.h"
#include "plugin_thread.h"

#include "common/attrs.h"
#include "params.h"

static llist events_bacon_freelist = LLIST_INITIALIZER(events_bacon_freelist);   // event_bacon
static event_bacon *events_bacon_array;
static struct futexlock events_bacon_lock;

static handler_context *events_would(events_context * restrict ctx, bamipc_event_type type)
{
  handler_context *handler;

  rcu_list_foreach(&g_handlers, handler, stk) {
    if(!(handler->event_mask & type)) {
      continue;
    }

    ctx->type = type;
    ctx->chan = 0;
    ctx->txq = 0;
    ctx->msg = 0;
    ctx->pb = 0;

    return handler;
  }

  return 0;
}

event_bacon *events_bacon_would(events_context * restrict ctx, bamipc_event_type type)
{
  handler_context *handler;

  switch(type) {
    /* these events have a binary payload and therefore cant be used here */
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_WAIT_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      RTA;
    default:
      break;
  }

  if(!(handler = events_would(ctx, type))) {
    return 0;
  }

  if(handler->chan) {
    ctx->chan = handler->chan;
    ctx->msg = channel_produce(ctx->chan);
    ctx->msg->type = BAMIPC_MSG_EVENTS;
    ctx->msg->evtype = type;

    /* in this case the caller operates on the message just like an event ; the structs
     * event_bacon and bampipc_message have binary compatibility for this reason */
    return (void*)ctx->msg;
  }

  /* handler without a channel (e.g. plugin thread) */
  ctx->txq = &handler->events_txq;
  ctx->txq_lock = &handler->events_txq_lock;
  ctx->txq_tid = handler->events_tid;
  ctx->pb = event_bacon_alloc();
  ctx->pb->evtype = type;

  return ctx->pb;
}

bamipc_message *events_binary_would(events_context * restrict ctx, bamipc_event_type type)
{
  handler_context *handler;

  switch(type) {
    /* these events have a binary payload */
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_WAIT_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      break;
    default:
      RTA;
  }

  if(!(handler = events_would(ctx, type))) {
    return 0;
  }

  ctx->chan = handler->chan;
  ctx->msg = channel_produce(ctx->chan);
  ctx->msg->type = BAMIPC_MSG_EVENTS;
  ctx->msg->evtype = type;

  return ctx->msg;
}

void events_publish(events_context * restrict ctx)
{
  bamipc_message *msg;
  handler_context *handler;
  event_bacon *pb;

  rcu_list_foreach(&g_handlers, handler, stk) {
    if(ctx->chan && ctx->chan == handler->chan) {
      continue;
    }
    if(ctx->txq && ctx->txq == &handler->events_txq) {
      continue;
    }

    if(!(handler->event_mask & ctx->type)) {
      continue;
    }

    if(handler->chan) {
      msg = channel_produce(handler->chan);
      msg->type = BAMIPC_MSG_EVENTS;
      msg->evtype = ctx->type;
      if(ctx->msg) {
        msg->id = ctx->msg->id;
        msg->code = ctx->msg->code;
        if((msg->size = ctx->msg->size)) {
          memcpy(msg->text, ctx->msg->text, ctx->msg->size);
        }
      } else {
        msg->id = ctx->pb->id;
        msg->code = ctx->pb->code;
        if((msg->size = ctx->pb->size)) {
          memcpy(msg->text, ctx->pb->text, ctx->pb->size);
        }
      }
      channel_post(handler->chan, msg);
    } else {
      pb = event_bacon_alloc();
      pb->evtype = ctx->type;
      if(ctx->msg) {
        RUNTIME_ASSERT(ctx->msg->size <= sizeof(pb->text));
        pb->id = ctx->msg->id;
        pb->code = ctx->msg->code;
        if((pb->size = ctx->msg->size)) {
          memcpy(pb->text, ctx->msg->text, ctx->msg->size);
        }
      } else {
        pb->id = ctx->pb->id;
        pb->code = ctx->pb->code;
        if((pb->size = ctx->pb->size)) {
          memcpy(pb->text, ctx->pb->text, ctx->pb->size);
        }
      }

      futexlock_acquire(&handler->events_txq_lock);
      llist_push(&handler->events_txq, &pb->lln);
      futexlock_release(&handler->events_txq_lock);
      uxtgkill(g_params.pid, handler->events_tid, SIGUSR1);
    }
  }

  if(ctx->chan) {
    channel_post(ctx->chan, ctx->msg);
  } else if(ctx->txq) {
    futexlock_acquire(ctx->txq_lock);
    llist_push(ctx->txq, &ctx->pb->lln);
    futexlock_release(ctx->txq_lock);
    uxtgkill(g_params.pid, ctx->txq_tid, SIGUSR1);
  } else if(ctx->pb) {
    /* system_error_event, and there are no handlers subscribed to this event */
    event_bacon_free(ctx->pb);
  } else {
    RTA;
  }
}

event_bacon *event_bacon_alloc(void)
{
  event_bacon *pb;

  futexlock_acquire(&events_bacon_lock);
  pb = llist_shift(&events_bacon_freelist, typeof(*pb), lln);
  futexlock_release(&events_bacon_lock);

  if(!pb) {
    fprintf(stderr, "event queue exhausted !\n");
    RTA;
  }

  return pb;
}

void event_bacon_free(event_bacon *pb)
{
  futexlock_acquire(&events_bacon_lock);
  llist_push(&events_bacon_freelist, &pb->lln);
  futexlock_release(&events_bacon_lock);
}

void event_bacon_free_list(llist * restrict list)
{
  futexlock_acquire(&events_bacon_lock);
  llist_splice_head(&events_bacon_freelist, list);
  futexlock_release(&events_bacon_lock);
}

void events_setup(void)
{
  int x;

  llist_init(&events_bacon_freelist);

  /* setup event queue */
  xmalloc(&events_bacon_array, sizeof(*events_bacon_array) * 512);
  for(x = 0; x < 512; x++) {
    llist_push(&events_bacon_freelist, &events_bacon_array[x].lln);
  }
}

void events_cleanup(void)
{
  free(events_bacon_array);
}
