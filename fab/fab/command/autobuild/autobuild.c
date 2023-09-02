/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "fab/client.h"
#include "fab/events.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"

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

static void attached(command * restrict cmd, fab_client * restrict client)
{

}

static void connected(command * restrict cmd, fab_client * restrict client)
{
  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;
  bacon_writer writer, *builder_writer;

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
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, request_narrator);

  /* re-configure goals */
  build_command_collate_goals(builder_writer, false);

  STRING("reconcile")

  /* invalidate targets */
  build_command_collate_invalidations(builder_writer);

  /* build the targets */
  STRING("autorun")

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(request_narrator, 0);
  narrator_sayc(request_narrator, 0);

  msg->size = nstor.l;
  client_post(client, msg);
}

static void disconnected(command * restrict cmd) { }

struct command autobuild_command = {
    name : "autobuild"
  , attached : attached
  , usage : build_command_usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
