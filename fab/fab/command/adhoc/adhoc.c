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

#include "fab/client.h"
#include "fab/events.h"
#include "fab/ipc.h"

#include "adhoc.h"

#include "zbuffer.h"
#include "params.h"

struct adhoc_args adhoc_args;
static uint64_t requestid;

static void usage(command * restrict cmd)
{
  printf(
"\n"
"adhoc [request]\n"
"\n"
  );
}

static void adhoc_connected(command * restrict cmd, fab_client * restrict client)
{
  fabipc_message * msg;
  size_t z;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += znloadw(msg->text + z, sizeof(msg->text) - z, adhoc_args.request.s, adhoc_args.request.len);
  z += znloadw(msg->text + z, sizeof(msg->text) - z, (char[]) { 0x00, 0x00 }, 2);
  msg->size = z;

  client_post(client, msg);
}

static void adhoc_process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == FABIPC_MSG_RESPONSE) {
    if(msg->code != 0) {
      fprintf(stderr, "response code %d msg %.*s\n", msg->code, msg->size, msg->text);
    }
    g_params.shutdown = true;
    return;
  }

  if(msg->type == FABIPC_MSG_RESULT) {
    write(1, msg->text, msg->size);
    write(1, "\n", 1);
  }
}

command adhoc_command = {
    name : "adhoc"
  , usage : usage
  , connected : adhoc_connected
  , process : adhoc_process
};
