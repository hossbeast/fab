/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "fab/ipc.h"
#include "fab/client.h"

#include "vars.h"
#include "params.h"
#include "zbuffer.h"

static uint64_t requestid;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"vars options\n"
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
  z += znloads(msg->text + z, sizeof(msg->text) - z, "vars-read");
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  client_post(client, msg);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  if(msg->id != requestid) {
    return;
  }

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    write(1, "\n", 1);
    return;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  write(1, msg->text, msg->size);
}

//
// public
//

struct command vars_command = {
    name : "vars"
  , usage : usage
  , connected : connected
  , process : process
};
