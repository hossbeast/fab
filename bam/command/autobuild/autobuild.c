/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "bam/client.h"
#include "bam/events.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "common/parseint.h"

#include "autobuild.h"
#include "build.h"

#include "info.h"
#include "params.h"
#include "params.h"

static uint64_t requestid;
static uint64_t eventsubid;

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  const char *pid_str;
  int64_t pid;
  const char *token_str;
  uint64_t token = 0;

  if(msg->type == BAMIPC_MSG_RESPONSE)
  {
    RUNTIME_ASSERT(msg->id == requestid || msg->id == eventsubid);
    return;
  }
  else if(msg->type == BAMIPC_MSG_RESULT)
  {
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_EVENTS);

  if(msg->evtype == BAMIPC_EVENT_SYSTEM_STATE_CHANGE)
  {

  }
  else if(msg->evtype == BAMIPC_EVENT_GOALS_SET)
  {
    /* goals will be considered to have been reassigned if the token was changed */
    if((token_str = strstr(msg->text, "token : 0x")) == 0) {
      goto reassigned;
    }
    if(parseuint(token_str + strlen("token : 0x"), PRIx64, 0, UINT64_MAX, 1, 20, &token, 0) != 0) {
      goto reassigned;
    }

    if((pid_str = strstr(msg->text, "pid : ")) == 0) {
      goto reassigned;
    }
    if(parseint(pid_str + strlen("pid : "), PRId64, 1, INT64_MAX, 1, 20, &pid, 0) != 0) {
      goto reassigned;
    }

    if(token == g_params.pid) {
      printf("goals set with matching token %"PRIx64"\n", token);
      return;
    }

reassigned:
    printf("goals reassigned by pid %"PRId64" with token %"PRIx64"\n", pid, token);
    printf("%.*s\n", msg->len, msg->text);
    g_params.shutdown = true;
  }
  else if(msg->evtype == BAMIPC_EVENT_AUTORUN_SET)
  {
    /* gratuitous autorun-set event */
  }
  else if(msg->evtype == BAMIPC_EVENT_BUILD_START)
  {
    /* reset exit status on new build */
    build_was_failed_slot = 0;
  }
  else if(msg->evtype == BAMIPC_EVENT_BUILD_END)
  {
  }
  else
  {
    build_command_process_event(client, msg);
  }
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  narrator *request_narrator;
  narrator_fixed fixed;
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;

  /* subscribe to relevant events */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    /* needed for build */
    | BAMIPC_EVENT_FORMULA_FORK_BIN
    | BAMIPC_EVENT_FORMULA_REAP_BIN
    | BAMIPC_EVENT_FORMULA_STDOUT_BIN
    | BAMIPC_EVENT_FORMULA_STDERR_BIN
    | BAMIPC_EVENT_FORMULA_AUXOUT_BIN
    | BAMIPC_EVENT_SYSTEM_STATE_CHANGE
    | BAMIPC_EVENT_BUILD_START
    | BAMIPC_EVENT_BUILD_END
    | BAMIPC_EVENT_RECONCILE_START
    | BAMIPC_EVENT_RECONCILE_END
    /* needed for autobuild */
    | BAMIPC_EVENT_GOALS_SET
    | BAMIPC_EVENT_AUTORUN_SET
    ;
  client_publish(client, msg);

  /* send the request */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  request_narrator = narrator_fixed_init(&fixed, msg->text, 0xfff);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, request_narrator);

  { MAPPING
    STRING("autorun")
    BOOL(true)
  }

  /* re-configure goals */
  build_command_collate_goals(MMS("bam-autobuild"), builder_writer);

  /* invalidate targets */
  build_command_collate_invalidations(builder_writer);

  /* having set the goals, autorun will trigger */

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(request_narrator, 0);
  narrator_sayc(request_narrator, 0);

  msg->len = fixed.l;
  client_publish(client, msg);
}

static void disconnected(command * restrict cmd)
{
}

struct command autobuild_command = {
    name : "autobuild"
  , attached : attached
  , usage : build_command_usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
