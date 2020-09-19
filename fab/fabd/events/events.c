/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#include <stdlib.h>

#include "valyria/stack.h"

#include "events.h"
#include "channel.h"
#include "handler_thread.h"
#include "handler.h"

bool events_would(fabipc_event_type type, handler_context ** restrict first, fabipc_message ** restrict msg, uint32_t * restrict tail)
{
  handler_context *handler;

  stack_foreach(&g_handlers, handler, stk) {
    if(!handler->chan) {
      continue;
    }

    if(!(handler->event_mask & (1 << (type - 1)))) {
      continue;
    }

    *first = handler;
    *msg = handler_produce(handler, tail);
    (*msg)->type = FABIPC_MSG_EVENTS;
    (*msg)->evtype = type;
    return true;
  }

  return false;
}

void events_publish(handler_context * first, fabipc_message * restrict firstmsg, uint32_t firsttail)
{
  fabipc_message *msg;
  handler_context *handler;
  uint32_t tail;

  stack_foreach(&g_handlers, handler, stk) {
    if(handler == first) {
      continue;
    }

    if(!handler->chan) {
      continue;
    }

    if(!(handler->event_mask & (1 << (firstmsg->evtype - 1)))) {
      continue;
    }

    msg = handler_produce(handler, &tail);
    msg->type = FABIPC_MSG_EVENTS;
    msg->evtype = firstmsg->evtype;
    msg->size = firstmsg->size;
    msg->id = firstmsg->id;
    if(firstmsg->size) {
      memcpy(msg->text, firstmsg->text, firstmsg->size);
    }

    handler_post(handler, tail);
  }

  handler_post(first, firsttail);
}
