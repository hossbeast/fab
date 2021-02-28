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
#include <inttypes.h>

#include "narrator.h"
#include "narrator/fixed.h"
//#include "valyria/list.h"
//#include "valyria/pstring.h"
#include "value/writer.h"
#include "logger/arguments.h"
#include "fab/ipc.h"
#include "fab/client.h"
#include "xlinux/xstdlib.h"

#include "common/attrs.h"

#include "describe.h"
#include "command.h"
#include "MAIN.errtab.h"
#include "params.h"
#include "args.h"

#include "zbuffer.h"
#include "marshal.h"

//static struct {
//  list * targets;
//} args;

struct describe_args describe_args;
static struct describe_args *args = &describe_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"usage : fab describe\n"
"\n"
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
  int x;

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;

  z = 0;
  z += znloads(msg->text + z, sizeof(msg->text) - z, "[ ");
  z += znloads(msg->text + z, sizeof(msg->text) - z, "select : [");

  for(x = 0; x < args->targets_len; x++)
  {
    z += znloadf(msg->text + z, sizeof(msg->text) - z, " path : \"%.*s\"", (int)args->targets[x].len, args->targets[x].s);
  }

  z += znloads(msg->text + z, sizeof(msg->text) - z, " ] ");
  z += znloads(msg->text + z, sizeof(msg->text) - z, "describe");
  z += znloads(msg->text + z, sizeof(msg->text) - z, " ]");

  // two terminating null bytes
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  z += znloadc(msg->text + z, sizeof(msg->text) - z, 0);
  msg->size = z;

  fab_client_post(client, msg);

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  //fab_global_stats stats;
  //descriptor_field *member;
  //int x;
  //size_t z;
  //void *src;
  //size_t sz;

  //src = msg->text;
  //sz = msg->size;
  //z = 0;
  //z += descriptor_type_unmarshal(&stats, &descriptor_fab_node_stats, src + z, sz - z);
  //RUNTIME_ASSERT(z == msg->size);

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    goto XAPI_FINALLY;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);

  printf("%s\n", msg->text);

  finally : coda;
}

//
// public
//

struct command describe_command = {
    usage : usage
  , connected : connected
  , process : process
//  , args_parse : parse_args
};

#if 0
static xapi parse_args(command * restrict cmd, int argc, char ** restrict argv)
{
  enter;

  int x;
  int longindex;
  pstring s = { };

  const struct option longopts[] = {
      { }
  };

  const char *switches =
    // no-argument switches
    ""

    // with-argument switches
    ""
  ;

  fatal(list_createx, &args.targets, 0, 0, wfree, 0);

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
  }

  while(optind < argc)
  {
    fatal(psinit, &s);
    fatal(pscats, &s, argv[optind++]);
    fatal(list_push, args.targets, s.s, 0);
    memset(&s, 0, sizeof(s));
  }

finally:
  fatal(psdestroy, &s);
coda;
}

xapi describe_command_cleanup()
{
  enter;

//  fatal(list_xfree, args.targets);

  finally : coda;
}
#endif
