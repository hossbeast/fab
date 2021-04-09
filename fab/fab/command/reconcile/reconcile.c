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

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

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

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE) {
    goto XAPI_FINALLY;
  }

  RUNTIME_ASSERT(msg->id == requestid);
  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESPONSE);

  g_params.shutdown = true;

  finally : coda;
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
