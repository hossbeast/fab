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

#include "narrator.h"
#include "narrator/fixed.h"
#include "fab/ipc.h"
#include "fab/client.h"

#include "bootstrap.h"
#include "command.h"
#include "params.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab bootstrap [ <selector>... ] ...\n"
"\n"
  );
}

//
// build
//

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;

  request_narrator = narrator_fixed_init(&nstor, msg->text, sizeof(msg->text));
  fatal(narrator_xsays, request_narrator, " bootstrap");

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;
  fab_client_post(client, msg);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  }

  finally : coda;
}

//
// public
//

struct command bootstrap_command = {
    usage : usage
  , connected : connected
  , process : process
};
