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

#include "fab/ipc.h"
#include "fab/events.h"

#include "events.h"
#include "channel.h"
#include "handler_thread.h"
#include "chan.h"
#include "rcu_list.h"

#include "common/attrs.h"
#include "params.h"

bool events_would(fabipc_event_type type, channel ** restrict first, fabipc_message ** restrict msg)
{
  channel *chan;

  rcu_list_foreach(&g_handlers, chan, stk) {
    //if(!chan) {
    //  continue;
    //}

    if(!(chan->event_mask & type)) {
      continue;
    }

    *first = chan;
    *msg = channel_produce(chan);
    (*msg)->type = FABIPC_MSG_EVENTS;
    (*msg)->evtype = type;
    return true;
  }

  return false;
}

void events_publish(channel * first, fabipc_message * restrict firstmsg)
{
  fabipc_message *msg;
  channel *chan;

  rcu_list_foreach(&g_handlers, chan, stk) {
    if(chan == first) {
      continue;
    }

    if(!(chan->event_mask & firstmsg->evtype)) {
      continue;
    }

    msg = channel_produce(chan);
    msg->type = FABIPC_MSG_EVENTS;
    msg->evtype = firstmsg->evtype;
    msg->size = firstmsg->size;
    msg->id = firstmsg->id;

    if(firstmsg->size) {
      memcpy(msg->text, firstmsg->text, firstmsg->size);
    }

    channel_post(chan, msg);
  }

  channel_post(first->chan, firstmsg);
}
