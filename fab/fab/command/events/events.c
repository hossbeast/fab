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

#include <stdio.h>
#include <inttypes.h>

#include "fab/ipc.h"
#include "fab/events.h"
#include "fab/client.h"
#include "common/attrs.h"

#include "events.h"
#include "command.h"

struct events_args events_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"events\n"
" stale\n"
" fresh\n"
" delete\n"
" global-invalidate\n"
" exec-fork\n"
" exec-wait\n"
" exec-stdout\n"
" exec-stderr\n"
" exec-auxout\n"
" goals\n"
" build-start\n"
" build-end\n"
"\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, fab_client * restrict client)
{
  fabipc_message * msg;

  /* subscribe to relevant events */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_EVENTSUB;
  msg->id = ++client->msgid;
  msg->attrs = events_args.event_mask;

  client_post(client, msg);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  switch(msg->evtype) {
    case FABIPC_EVENT_FORMULA_EXEC_FORKED:
    case FABIPC_EVENT_FORMULA_EXEC_WAITED:
    case FABIPC_EVENT_FORMULA_EXEC_STDOUT:
    case FABIPC_EVENT_FORMULA_EXEC_STDERR:
    case FABIPC_EVENT_FORMULA_EXEC_AUXOUT:
      /* these events carry a build_slot object */
      printf("0x%-16"PRIx64" %s\n", msg->id, attrs32_name_byvalue(fabipc_event_type_attrs, msg->evtype));
      break;
    default:
      printf("0x%-16"PRIx64" %s %.*s\n", msg->id, attrs32_name_byvalue(fabipc_event_type_attrs, msg->evtype), msg->size, msg->text);
  }
}

//
// public
//

struct command events_command = {
    name : "events"
  , usage : usage
  , connected : connected
  , process : process
};
