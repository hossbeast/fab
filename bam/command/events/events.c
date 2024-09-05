/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "bam/ipc.h"
#include "bam/events.h"
#include "bam/client.h"
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

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;

  /* subscribe to relevant events */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  msg->id = ++client->msgid;
  msg->attrs = events_args.event_mask;

  client_post(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  switch(msg->evtype) {
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_WAIT_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      /* these events carry a build_slot object */
      printf("0x%-16"PRIx64" %s\n", msg->id, attrs32_name_byvalue(&bamipc_event_type_attrs, msg->evtype));
      break;
    default:
      printf("0x%-16"PRIx64" %s %.*s\n", msg->id, attrs32_name_byvalue(&bamipc_event_type_attrs, msg->evtype), msg->size, msg->text);
  }
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

//
// public
//

static void disconnected(command * restrict cmd) { }
struct command events_command = {
    name : "events"
  , attached : attached
  , usage : usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
