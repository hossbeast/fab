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
