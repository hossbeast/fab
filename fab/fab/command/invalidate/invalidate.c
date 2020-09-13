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

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "value/writer.h"
#include "logger/arguments.h"
#include "fab/ipc.h"
#include "fab/client.h"

#include "common/attrs.h"

#include "invalidate.h"
#include "command.h"
#include "MAIN.errtab.h"
#include "params.h"
#include "args.h"

static struct {
  list * targets;
} args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab invalidate [ <selector>... ] ...\n"
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

  fatal(list_create, &args.targets);

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
      fatal(list_push, args.targets, optarg, 0);
    }
  }

  // options following --
  for(; optind < argc; optind++)
  {
    fatal(list_push, args.targets, argv[optind], 0);
  }

  finally : coda;
}

static xapi request_write(narrator * restrict N)
{
  enter;

  int x;
  char *arg;

  if(g_args.invalidate) {
    fatal(narrator_xsays, N, " global-invalidate");
    goto XAPI_FINALIZE;
  }

  fatal(narrator_xsays, N, ""
" select : ["
  );

  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    fatal(narrator_xsayf, N, ""
" pattern : %s", arg
    );
  }

  fatal(narrator_xsays, N, ""
" ] "
" invalidate "
  );

  finally : coda;
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
  msg = fab_client_produce(client, 0);
  msg->type = FABIPC_MSG_REQUEST;

  request_narrator = narrator_fixed_init(&nstor, msg->text, sizeof(msg->text));
  fatal(request_write, request_narrator);

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;
  fab_client_post(client);

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

xapi invalidate_command_cleanup()
{
  enter;

  fatal(list_xfree, args.targets);

  finally : coda;
}

struct command invalidate_command = {
    args_parse : parse_args
  , usage : usage
  , connected : connected
  , process : process
};
