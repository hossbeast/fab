/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "fab/ipc.h"
#include "fab/client.h"
#include "fab/events.h"

#include "reconcile.h"
#include "params.h"
#include "zbuffer.h"

static uint64_t requestid;

static void usage(command * restrict cmd)
{
  printf(
"\n"
"reconcile options\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, fab_client * restrict client)
{
  fabipc_message * msg;
  size_t z;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += znloads(msg->text + z, sizeof(msg->text) - z, "reconcile");

  // two terminating null bytes
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  client_post(client, msg);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  if(msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE) {
    return;
  }

  RUNTIME_ASSERT(msg->id == requestid);
  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESPONSE);

  g_params.shutdown = true;
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
