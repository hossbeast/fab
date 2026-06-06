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
"events positional args\n"
" -a                          show all events\n"
" <event>                     show this event\n"
"\n"
"events list (text payload)\n"
" stale                       node becomes invalidated\n"
" fresh                       node is reconciled\n"
" create                      node created\n"
" delete                      node deleted\n"
" goals-set                   goals are set\n"
" autorun-set                 autorun is set\n"
" system-state-change         global system state change\n"
" build-start                 build begins\n"
" build-end                   build completes\n"
" reconcile-start             reconciliation begins\n"
" reconcile-end               reconciliation completes\n"
" formula-fork                formula execution begins\n"
" formula-reap                formula execution completes\n"
" rma-start                   rule execution begins\n"
" rma-end                     rule execution completes\n"
" rma-search                  rule search pattern nodes\n"
" rma-generate                rule generate pattern nodes\n"
" rma-resolve                 bind rule namespace resolution\n"
" rma-bind                    bind rule attachments\n"
"\n"
"events list (binary payload)\n"
" exec-fork                   formula child forked\n"
" exec-wait                   formula child waited\n"
" exec-stdout                 formula child writes to stdout\n"
" exec-stderr                 formula child writes to stderr\n"
" exec-auxout                 formula child writes to auxout\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;

  /* subscribe to relevant events */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  msg->id = ++client->msgid;
  msg->attrs = events_args.event_mask;

  client_publish(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  switch(msg->evtype) {
    case BAMIPC_EVENT_FORMULA_FORK_BIN:
    case BAMIPC_EVENT_FORMULA_REAP_BIN:
    case BAMIPC_EVENT_FORMULA_STDOUT_BIN:
    case BAMIPC_EVENT_FORMULA_STDERR_BIN:
    case BAMIPC_EVENT_FORMULA_AUXOUT_BIN:
      /* these events carry a build_slot object */
      printf("0x%-16"PRIx64" %s\n", msg->id, attrs32_name_byvalue(&bamipc_event_type_attrs, msg->evtype));
      break;
    default:
      /* all other events carry bacon text or freetext */
      printf("%.*s\n", msg->len, msg->text);
      break;
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
