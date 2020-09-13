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

#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/mempolicy.h"
#include "value/writer.h"
#include "narrator.h"
#include "narrator/units.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "logger.h"
#include "logger/expr.h"
#include "logger/category.h"
#include "logger/arguments.h"
#include "valyria/list.h"
#include "fab/ipc.h"

#include "args.h"
#include "logging.h"
#include "params.h"
#include "errtab/MAIN.errtab.h"
#include "build.h"
#include "autobuild.h"
#include "adhoc.h"
#include "invalidate.h"
#include "events.h"
#include "command.h"

#include "macros.h"
#include "common/assure.h"

static int argc;
static char **argv;

struct g_args_t g_args;

//
// public
//

void args_version()
{
  printf(
" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
"+DEVEL"
#elif DEBUG
"+DEBUG"
#endif
" @ " XQUOTE(BUILDSTAMP)
"\n"
  );

  exit(0);
}

void args_usage(command * restrict cmd)
{
  printf(
"fab : build optimally\n"
"\n"
"usage: fab [command | option]..\n"
"\n"
"commands\n"
" adhoc                 adhoc request\n"
" autobuild             as build, but remain connected\n"
" build       (default) bring targets up-to-date\n"
" events                listen for and print events\n"
" invalidate            invalidate nodes\n"
"\n"
"global options\n"
" --help | -h       this message\n"
" --version | -V    print version information\n"
" -B                first, global invalidation\n"
" -K                first, kill extant fabd if any\n"
  );

  if(cmd) {
    printf("\n");
    cmd->usage(cmd);
  }

  printf(
"\n"
"https://github.com/hossbeast/fab\n"
"\n"
  );

  exit(0);
}

static command *command_lookup(const char *s)
{
  if(strcmp(s, "build") == 0) {
    return &build_command;
  } else if(strcmp(s, "autobuild") == 0) {
    return &autobuild_command;
  } else if(strcmp(s, "adhoc") == 0) {
    return &adhoc_command;
  } else if(strcmp(s, "invalidate") == 0) {
    return &invalidate_command;
  } else if(strcmp(s, "events") == 0) {
    return &events_command;
  }

  return 0;
}

xapi args_parse(command ** cmdp)
{
  enter;

  int x;
  int longindex;
  int help = 0;
  int version = 0;
  command *cmd = 0;

  const struct option longopts[] = {
      { "help"    , no_argument , &help, 1 }
    , { "version" , no_argument , &version, 1 }
  };

  const char *switches =
    // no-argument switches
    "hVBK"

    // with-argument switches
    ""
  ;

  fatal(xmalloc, &argv, sizeof(*argv) * (g_argc + 1));
  argc = 1;

//printf("g-argv %zu\n", g_argc);
//for(x = 0; x < g_argc; x++) {
//  printf(" %2d %s\n", x, g_argv[x]);
//}

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(g_argc, g_argv, switches, longopts, &longindex)) != -1)
  {
//printf("x %d optind %d\n", x, optind);
    // longopts
    if(x == 0)
    {
    }
    else if(x == 'h')
    {
      help = true;
    }
    else if(x == 'V')
    {
      version = true;
    }
    else if(x == 'B')
    {
      g_args.invalidate = true;
    }
    else if(x == 'K')
    {
      g_args.kill = true;
    }
    else if(!cmd)
    {
      if((cmd = command_lookup(g_argv[optind - 1])) == 0)
      {
        fails(MAIN_BADARGS, "unknown command", g_argv[optind - 1]);
      }
    }
    else
    {
      argv[argc++] = g_argv[optind - 1];
    }
  }

  for(; optind < g_argc; optind++) {
    if(cmd) {
      argv[argc++] = g_argv[optind];
      continue;
    }
    if((cmd = command_lookup(g_argv[optind])) == 0)
    {
      fails(MAIN_BADARGS, "unknown command", g_argv[optind]);
    }
  }

  if(help)
  {
    args_usage(cmd);
  }
  if(version)
  {
    args_version();
  }

  if(!cmd) {
    cmd = &build_command;
  }

  argv[0] = cmd->name;

//printf("sub-argv %u\n", argc);
//for(x = 0; x < argc; x++) {
//  printf(" %2d %s\n", x, argv[x]);
//}

  fatal(cmd->args_parse, cmd, argc, argv);

  *cmdp = cmd;

  finally : coda;
}

xapi args_teardown(void)
{
  enter;

  wfree(argv);

  finally : coda;
}

#if 0
xapi args_collate(const command * restrict cmd, fabipc_message * msg)
{
  enter;

  value_writer writer;
  narrator * request_narrator;
  narrator_fixed nstor;

  value_writer_init(&writer);

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);

  fatal(value_writer_open, &writer, request_narrator);
  fatal(cmd->collate, &writer);
  fatal(value_writer_close, &writer);

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;

//  // save a spot for the message length
//  request_narrator = narrator_fixed_init(nstor, msg->text, 0xfff);
//  fatal(narrator_xsayw, request_narrator, (char[]) { 0xde, 0xad, 0xbe, 0xef }, 4);
  // stitch up the message length
//  size_t message_size;
//  message_size =
//  message_len = message_size - 4;
//  fatal(narrator_xseek, request_narrator, 0, NARRATOR_SEEK_SET, 0);
//  fatal(narrator_xsayw, request_narrator, &message_len, sizeof(message_len));

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

xapi args_report(const command * restrict cmd)
{
  enter;

  narrator_growing * N = 0;

  fatal(narrator_growing_create, &N);

  fatal(narrator_xsays, &N->base, "fab");
  fatal(cmd->command_say, &N->base);
  fatal(narrator_xsayf, &N->base, " %s", g_logvs);
  logs(L_ARGS, N->s);

finally:
  if(N) {
    fatal(narrator_xfree, &N->base);
  }
coda;
}

#if 0
/// args_report
//
// SUMMARY
//  log a summary of args as-parsed
//
xapi args_report(const struct command * restrict cmd)
  __attribute__((nonnull));
#endif

/// args_request_collate
//
// SUMMARY
//  build a fab request from g_args
//
// PARAMETERS
//  request - (returns) request
//
xapi args_collate(const struct command * restrict cmd, struct fabipc_message *msg)
  __attribute__((nonnull));
#endif
