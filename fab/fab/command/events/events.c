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

#include "xapi.h"
#include "types.h"

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "value/writer.h"
#include "logger/arguments.h"
#include "fab/ipc.h"
#include "fab/client.h"

#include "common/attrs.h"

#include "events.h"
#include "command.h"
#include "MAIN.errtab.h"
#include "params.h"
#include "args.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab events [ <selector>... ] ...\n"
"\n"
  );
}

static xapi parse_args(command * restrict cmd, int argc, char ** restrict argv)
{
  enter;

  int x;
  int longindex;

  const struct option longopts[] = {
      { }
  };

  const char *switches =
    // no-argument switches
    ""

    // with-argument switches
    ""
  ;

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(argc, argv, switches, longopts, &longindex)) != -1)
  {
    if(x == 0)
    {
      // longopts
    }
    else if(x == '?')
    {
      if(optopt)
      {
        failf(MAIN_BADARGS, "unknown switch", "-%c", optopt);
      }
      else
      {
        fails(MAIN_BADARGS, "unknown argument", argv[optind-1]);
      }
    }
    else
    {
      // unknown optarg
    }
  }

  // options following --
  for(; optind < argc; optind++)
  {
    // unknown argv[optind]
  }

  finally : coda;
}

//
// build
//

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  fabipc_message * msg;

  /* subscribe to relevant events */
  msg = fab_client_produce(client, 0);
  msg->type = FABIPC_MSG_EVENTSUB;
  msg->attrs = 0
    | 1 << (FABIPC_EVENT_NODE_STALE - 1)
    | 1 << (FABIPC_EVENT_NODE_FRESH - 1)
    | 1 << (FABIPC_EVENT_GLOBAL_INVALIDATION - 1)
    | 1 << (FABIPC_EVENT_FORMULA_EXEC_FORKED - 1)
    | 1 << (FABIPC_EVENT_FORMULA_EXEC_WAITED - 1)
    | 1 << (FABIPC_EVENT_FORMULA_EXEC_STDOUT - 1)
    | 1 << (FABIPC_EVENT_FORMULA_EXEC_STDERR - 1)
    | 1 << (FABIPC_EVENT_FORMULA_EXEC_AUXOUT - 1)
    | 1 << (FABIPC_EVENT_GOALS - 1)
    | 1 << (FABIPC_EVENT_BUILD_START - 1)
    | 1 << (FABIPC_EVENT_BUILD_END - 1)
    ;
  fab_client_post(client);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  printf("%10u %s %.*s\n", msg->id, attrs32_name_byvalue(fabipc_event_type_attrs, msg->evtype), msg->size, msg->text);

  finally : coda;
}

//
// public
//

xapi events_command_cleanup()
{
  enter;

  finally : coda;
}

struct command events_command = {
    name : "events"
  , args_parse : parse_args
  , usage : usage
  , connected : connected
  , process : process
};
