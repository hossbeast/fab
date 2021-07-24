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

#include "fab/client.h"
#include "fab/events.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "autobuild.h"
#include "build.h"

#include "params.h"

static uint64_t requestid;
static uint64_t eventsubid;

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  uint32_t pid;

  if(msg->type == FABIPC_MSG_RESPONSE)
  {
    RUNTIME_ASSERT(msg->id == requestid || msg->id == eventsubid);
    return;
  }
  else if(msg->type == FABIPC_MSG_RESULT)
  {
    return;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_EVENTS);

  if(msg->evtype == FABIPC_EVENT_SYSTEM_STATE) { }
  else if(msg->evtype == FABIPC_EVENT_GOALS)
  {
    RUNTIME_ASSERT((msg->id & UINT32_MAX) == 1);
    pid = PID_FROM_MSGID(msg->id);
    if(pid != g_params.pid)
    {
      printf("goals reassigned by %"PRIu32"\n", pid);
      g_params.shutdown = true;
    }
  }
  else
  {
    build_command_process_event(client, msg);
  }
}

static void connected(command * restrict cmd, fab_client * restrict client)
{
  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;

  /* subscribe to relevant events */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | FABIPC_EVENT_FORMULA_EXEC_FORKED
    | FABIPC_EVENT_FORMULA_EXEC_STDOUT
    | FABIPC_EVENT_FORMULA_EXEC_STDERR
    | FABIPC_EVENT_FORMULA_EXEC_AUXOUT
    | FABIPC_EVENT_FORMULA_EXEC_WAITED
    | FABIPC_EVENT_GOALS
    ;
  client_post(client, msg);

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = PID_AS_MSGID(getpid());

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);

  /* re-configure goals */
  build_command_collate_goals(request_narrator, true);

  narrator_xsayf(request_narrator, " reconcile ");

  /* invalidate targets */
  build_command_collate_invalidations(request_narrator);

  /* build the targets */
  narrator_xsayf(request_narrator, " autorun");

  // two terminating null bytes
  narrator_xsayw(request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;

  client_post(client, msg);
}

struct command autobuild_command = {
    name : "autobuild"
  , usage : build_command_usage
  , connected : connected
  , process : process
};
