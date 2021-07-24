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

#include <stdio.h>

#include "fab/client.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "touch.h"
#include "command.h"
#include "params.h"

static uint64_t requestid;
struct touch_args touch_args;
static struct touch_args *args = &touch_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"touch [<path>...]\n"
  );
}

static void request_write(narrator * restrict N)
{
  int x;

  if(g_args.invalidate) {
    narrator_xsays(N, " global-invalidate");
    return;
  }

  narrator_xsays(N, ""
" select : ["
  );

  for(x = 0; x < args->targets_len; x++)
  {
    narrator_xsayf(N, ""
" path : \"%.*s\"", (int)args->targets[x].len, args->targets[x].s
    );
  }

  narrator_xsays(N, ""
" ] "
" invalidate "
  );
}

//
// build
//

static void connected(command * restrict cmd, fab_client * restrict client)
{
  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;

  /* if no targets specified, invalidate everything */
  if(args->targets_len == 0)
  {
    g_args.invalidate = true;
  }

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  request_narrator = narrator_fixed_init(&nstor, msg->text, sizeof(msg->text));
  request_write(request_narrator);

  // two terminating null bytes
  narrator_xsayw(request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;
  client_post(client, msg);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  }
}

//
// public
//

struct command touch_command = {
    usage : usage
  , connected : connected
  , process : process
};
