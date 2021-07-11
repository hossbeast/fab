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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "xapi.h"
#include "errtab/MAIN.errtab.h"
#include "xapi/SYS.errtab.h"
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "logger.h"
#include "logger/arguments.h"
#include "logger/expr.h"

#include "args.h"
#include "logging.h"

#include "macros.h"
#include "common/assure.h"
#include "git-state.h"

struct g_args_t g_args;

static void args_version()
{
  printf("xunit test runner  %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void args_usage()
{
  printf(
"xunit test runner %s\n"
"\n"
"usage : xunit [ [ option ] [ logexpr ] [ /path/to/object ] ] ...\n"
"\n"
" --help    : print this message\n"
" --version : print version information\n"
"\n"
"options\n"
" --nofork  : don't fork before each test\n"
"\n"
"build optimally      https://github.com/hossbeast/fab\n"
    , git_describe
  );

  exit(0);
}

static xapi objects_push(char * path, size_t * objectsa)
{
  enter;

  char space[512];
  uint16_t len;

  fatal(assure, &g_args.objects, sizeof(*g_args.objects), g_args.objectsl + 1, objectsa);

  // arguments to dlopen are specified as paths
  if(strstr(path, "/"))
  {
    fatal(ixstrdup, &g_args.objects[g_args.objectsl++], path);
  }
  else
  {
    len = strlen(path);
    if(len > 500) {
      fail(SYS_ABORT);
    }
    memcpy(space, "./", 2);
    memcpy(space + 2, path, len);
    space[len + 2] = 0;
    fatal(ixstrdup, &g_args.objects[g_args.objectsl++], space);
  }

  finally : coda;
}

xapi args_parse()
{
  enter;

  int help = 0;
  int version = 0;
  size_t objectsa = 0;
  int fork = 1;

  struct option longopts[] = {
      { "help"                        , no_argument       , &help, 1 }
    , { "args"                        , no_argument       , &help, 1 }
    , { "params"                      , no_argument       , &help, 1 }
    , { "options"                     , no_argument       , &help, 1 }
    , { "opts"                        , no_argument       , &help, 1 }
    , { "version"                     , no_argument       , &version, 1 }
    , { "vers"                        , no_argument       , &version, 1 }
    , { "vrs"                         , no_argument       , &version, 1 }
    , { "fork"                        , no_argument       , &fork, 1 }
    , { "nofork"                      , no_argument       , &fork, 0 }
    , { "no-fork"                     , no_argument       , &fork, 0 }
    , { }
  };

  char * switches =
    // getopt options
    "-"

    // no-argument switches
    "hV"

    // with-argument switches
    ""
  ;

  int x;
  int indexptr;
  opterr = 0;
  while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], switches, longopts, &indexptr)) != -1)
  {
    if(x == 0)
    {
      // longopts - placeholder
    }
    else if(x == 'h')
    {
      help = 1;
    }
    else if(x == 'V')
    {
      version = 1;
    }
    else if(x == '?')
    {
      // unrecognized argv element
      if(optopt)
      {
        failf(MAIN_NXSWITCH, "switch", "-%c", optopt);
      }
      else
      {
        failf(MAIN_BADARGS, "argument", "%s", g_argv[optind-1]);
      }
    }
    else
    {
      // non-option argv elements
      fatal(objects_push, optarg, &objectsa);
    }
  }

  for(; optind < g_argc; optind++)
  {
    // options following --
    fatal(objects_push, g_argv[optind], &objectsa);
  }

  g_args.fork = fork;

  if(help)
  {
    args_usage();
  }
  if(version)
  {
    args_version();
  }

  finally : coda;
}

xapi args_summarize()
{
  enter;

  int x;

  logs(L_ARGS, "--------------------------------------------------------------------------------");

  logf(L_ARGS, " %s (  %c  ) fork %21s %s", g_args.fork ? "*" : "", ' ', "", g_args.fork ? "true" : "false");

  if(g_args.objectsl == 0)
    logf(L_ARGS, " %s (  %c  ) object(s)", " ", ' ');
  for(x = 0; x < g_args.objectsl; x++)
    logf(L_ARGS, " %s (  %c  ) object(s) %16s %s", "*", ' ', "", g_args.objects[x]);

  logs(L_ARGS, "--------------------------------------------------------------------------------");

  finally : coda;
}

void args_teardown()
{
  int x;
  for(x = 0; x < g_args.objectsl; x++)
    wfree(g_args.objects[x]);

  wfree(g_args.objects);
}
