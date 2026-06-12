/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/events.h"

#include "info.h"
#include "params.h"
#include "zcat.h"

static uint64_t requestid;
static uint64_t eventsubid;

static void usage(command * restrict cmd)
{
  printf(
"bam reconcile - reload everything\n"
"\n"
"usage: bam reconcile\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  size_t z;

  /* subscribe to relevant events */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | BAMIPC_EVENT_SYSTEM_STATE_CHANGE
    | BAMIPC_EVENT_RECONCILE_START
    | BAMIPC_EVENT_RECONCILE_END
    ;
  client_publish(client, msg);

  /* send the request */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += zcats(msg->text + z, msg->size - z, "reconcile");

  // two terminating null bytes
  z += zcatc(msg->text + z, msg->size - z, 0);
  z += zcatc(msg->text + z, msg->size - z, 0);
  msg->len = z;

  client_publish(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->type == BAMIPC_MSG_EVENTS) {
    /* wait until the reconcile-end event, which will happen even if there is an error
     * emitted during the reconcile process */
    if(msg->evtype == BAMIPC_EVENT_RECONCILE_END) {
      g_params.shutdown = true;
    }
    return;
  }

  RUNTIME_ASSERT(msg->id == requestid || msg->id == eventsubid);
  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESPONSE);
}

//
// public
//

struct command reconcile_command = {
    name : "reconcile"
  , usage : usage
  , connected : connected
  , process : process
};
