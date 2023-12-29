/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/events.h"

#include "reconcile.h"
#include "params.h"
#include "zcat.h"

static uint64_t requestid;

static void usage(command * restrict cmd)
{
  printf(
"reconcile options (none)\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  size_t z;

  /* send the request */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += zcats(msg->text + z, sizeof(msg->text) - z, "reconcile");

  // two terminating null bytes
  z += zcatc(msg->text + z, sizeof(msg->text) - z, 0);
  z += zcatc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  client_post(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->type == BAMIPC_MSG_EVENTS && msg->evtype == BAMIPC_EVENT_SYSTEM_STATE) {
    return;
  }

  RUNTIME_ASSERT(msg->id == requestid);
  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESPONSE);

  g_params.shutdown = true;
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

//
// public
//

static void disconnected(command * restrict cmd) { }
struct command reconcile_command = {
    name : "reconcile"
  , attached : attached
  , usage : usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
