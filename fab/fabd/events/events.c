/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/events.h"
#include "bacon/writer.h"

#include "events.h"
#include "channel.h"
#include "handler_thread.h"
#include "rcu_list.h"
#include "handler.h"

#include "common/attrs.h"
#include "params.h"

bool events_would(
    bamipc_event_type type
  , channel * restrict * restrict first
  , bamipc_message ** restrict msg
)
{
  handler_context *handler;

  rcu_list_foreach(&g_handlers, handler, stk) {
    if(!handler->chan) {
      continue;
    }

    if(!(handler->chan->event_mask & type)) {
      continue;
    }

    *first = handler->chan;
    *msg = channel_produce(handler->chan);
    (*msg)->type = BAMIPC_MSG_EVENTS;
    (*msg)->evtype = type;
    return true;
  }

  return false;
}

void events_publish(channel * restrict first, bamipc_message * restrict firstmsg)
{
  bamipc_message *msg;
  handler_context *handler;

  rcu_list_foreach(&g_handlers, handler, stk) {
    if(!handler->chan) {
      continue;
    }

    if(handler->chan == first) {
      continue;
    }

    if(!(handler->chan->event_mask & firstmsg->evtype)) {
      continue;
    }

    msg = channel_produce(handler->chan);
    msg->type = BAMIPC_MSG_EVENTS;
    msg->evtype = firstmsg->evtype;
    msg->size = firstmsg->size;
    msg->id = firstmsg->id;

    if(firstmsg->size) {
      memcpy(msg->text, firstmsg->text, firstmsg->size);
    }

    channel_post(handler->chan, msg);
  }

  if(first) {
    channel_post(first, firstmsg);
  }

  msg = firstmsg;
  if(msg->code != 0) {
    printf("%5u %.*s\n", msg->code, (int)msg->size, msg->text);
  }

  if(!first) {
    free(msg);
  }
}
