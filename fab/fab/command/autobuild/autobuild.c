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

#include <getopt.h>
#include <signal.h>

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "narrator.h"
#include "narrator/units.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "logger/arguments.h"
#include "yyutil/box.h"

#include "autobuild.h"
#include "build.h"
#include "command.h"
#include "errtab/MAIN.errtab.h"
#include "fab/client.h"
#include "fab/ipc.h"
#include "config.internal.h"

#include "macros.h"
#include "common/attrs.h"
#include "params.h"

//
// autobuild
//

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type != FABIPC_MSG_EVENTS)
    goto XAPI_FINALIZE;

  if(msg->evtype == FABIPC_EVENT_GOALS)
  {
    if(msg->id != getpid()) {
      g_params.shutdown = true;
    }
  }
  else
  {
    fatal(build_command_process_event, client, msg);
  }

  finally : coda;
}

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;

  /* subscribe to relevant events */
  msg = fab_client_produce(client, 0);
  msg->type = FABIPC_MSG_EVENTSUB;
  msg->attrs = 0
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_FORKED - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_STDOUT - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_STDERR - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_AUXOUT - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_WAITED - 1))
    | (1 << (FABIPC_EVENT_GOALS - 1))
    ;
  fab_client_post(client);

  /* send the request */
  msg = fab_client_produce(client, 0);
  msg->type = FABIPC_MSG_REQUEST;
  msg->id = getpid();

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(build_command_request_collate, request_narrator);
  fatal(narrator_xsays, request_narrator, " autorun");

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;

  fab_client_post(client);

  finally : coda;
}

struct command autobuild_command = {
    name : "autobuild"
  , args_parse : build_command_args_parse
  , usage : build_command_usage
  , connected : connected
  , process : process
};
