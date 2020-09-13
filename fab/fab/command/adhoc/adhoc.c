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

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/units.h"
#include "narrator/fixed.h"
#include "value.h"
#include "value/writer.h"
#include "value/parser.h"

#include "adhoc.h"
#include "command.h"
#include "errtab/MAIN.errtab.h"
#include "fab/client.h"
#include "fab/ipc.h"

#include "macros.h"
#include "common/attrs.h"

static struct {
  char * request;
} args;

xapi adhoc_command_cleanup()
{
  enter;

  wfree(args.request);

  finally : coda;
}

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab adhoc [request] ...\n"
"\n"
  );
}

static xapi args_parse(command * restrict cmd, int argc, char ** restrict argv)
{
  enter;

  int x;
  value_parser *parser = 0;
  value *val;

  struct option longopts[] = {
      { }
  };

  // disable getopt error messages
  opterr = 0;

  int longindex;
  while((x = getopt_long(argc, (void*)argv, "-", longopts, &longindex)) != -1)
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
      fatal(ixstrcatf, &args.request, " %s", optarg);
    }
  }

  // options following --
  for(; optind < argc; optind++)
  {
    fatal(ixstrcatf, &args.request, " %s", argv[optind]);
  }

  // verify that the request at least parses locally
  fatal(value_parser_create, &parser);
  fatal(value_parser_parse, parser, MMS(args.request), "-args-", VALUE_TYPE_LIST, &val);

finally:
  fatal(value_parser_xfree, parser);
coda;
}

static xapi adhoc_connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  value_writer writer;
  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;

  /* subscribe to relevant events */
  msg = fab_client_produce(client, 0);
  msg->type = FABIPC_MSG_EVENTSUB;
  msg->attrs = 0
//    | (1 << (FABIPC_EVENT_INVALIDATED - 1))
//    | (1 << (FABIPC_EVENT_UPDATED - 1))
//    | (1 << (FABIPC_EVENT_FORMULA_EXEC - 1))
//    | (1 << (FABIPC_EVENT_FORMULA_EXEC_STDOUT - 1))
//    | (1 << (FABIPC_EVENT_FORMULA_EXEC_STDERR - 1))
//    | (1 << (FABIPC_EVENT_FORMULA_EXEC_AUXOUT - 1))
    ;
  fab_client_post(client);

  /* send the request */
  msg = fab_client_produce(client, 0);
  msg->type = FABIPC_MSG_REQUEST;

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);

  fatal(narrator_xsays, request_narrator, args.request);

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;
  fab_client_post(client);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

static xapi adhoc_process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type == FABIPC_MSG_STDOUT) {
printf("server/stdout: %.*s\n", msg->size, msg->text);
  } else if(msg->type == FABIPC_MSG_STDERR) {
printf("server/stderr: %.*s\n", msg->size, msg->text);
  } else if(msg->type == FABIPC_MSG_RESULT) {
printf("result: %.*s\n", msg->size, msg->text);
  } else if(msg->type == FABIPC_MSG_EVENTS) {
printf("events: %s %.*s\n", attrs32_name_byvalue(fabipc_event_type_attrs, msg->evtype), msg->size, msg->text);
  } else if(msg->type == FABIPC_MSG_RESPONSE) {
printf("response: %d - %s\n", msg->code, msg->text);
//    if(msg->code) {
//      receiving = true; // false;
//    }
  } else {
printf("WTF\n");
  }

  finally : coda;
}

command adhoc_command = {
    name : "adhoc"
  , args_parse : args_parse
  , usage : usage
  , connected : adhoc_connected
  , process : adhoc_process
};
