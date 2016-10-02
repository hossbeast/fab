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
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "logger.h"
#include "logger/arguments.h"
#include "logger/filter.h"

#include "args.h"
#include "logging.h"

#include "macros.h"
#include "assure.h"

struct g_args_t g_args;

static xapi usage(int valid, int version, int help, int logs)
{
  enter;

  printf(
"xunit : test runner\n"
);
if(version)
{
  printf(" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
  "+DEVEL"
#elif DEBUG
  "+DEBUG"
#endif
    " @ " XQUOTE(BUILDSTAMP)
    "\n"
  );
}
if(help)
{
  printf(
"\n"
"usage : xunit [ [ option ] [ logexpr ] [ /path/to/object ] ] ...\n"
"\n"
" --help    : this message\n"
" --version : version information\n"
" --logs    : logger configuration\n"
"\n"
"----------------- [ options ] --------------------------------------------------------------------\n"
"\n"
#if DEBUG || DEVEL
" --backtrace-pithy   (default) produce a summary of the callstack upon failure\n"
" --backtrace-full              produce a complete description of the callstack upon failure\n"
#endif
  );
}

if(logs)
{
printf(
"\n"
"----------------- [ logs ] -----------------------------------------------------------------------\n"
"\n"
);

  fatal(logger_filter_pushs, 0, "+LOGGER");
  fatal(logger_categories_report);
  fatal(logger_filter_pop, 0);
}

printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
);

exit(!valid);

  finally : coda;
}

static xapi objects_push(char * path, size_t * objectsa)
{
  enter;

  // arguments to dlopen are specified as paths
  if(strstr(path, "/") == 0)
  {
    failf(MAIN_NOTPATH, "argument", "%s", path);
  }

  fatal(assure, &g_args.objects, sizeof(*g_args.objects), g_args.objectsl + 1, objectsa);
  fatal(ixstrdup, &g_args.objects[g_args.objectsl++], path);

  finally : coda;
}

xapi args_parse()
{
  enter;

  int help = 0;
  int version = 0;
  int logs = 0;
  size_t objectsa = 0;

  struct option longopts[] = {
      { "help"                        , no_argument       , &help, 1 }
    , { "args"                        , no_argument       , &help, 1 }
    , { "params"                      , no_argument       , &help, 1 }
    , { "options"                     , no_argument       , &help, 1 }
    , { "opts"                        , no_argument       , &help, 1 }
    , { "version"                     , no_argument       , &version, 1 }
    , { "vers"                        , no_argument       , &version, 1 }
    , { "vrs"                         , no_argument       , &version, 1 }
    , { "log"                         , no_argument       , &logs, 1 }
    , { "logs"                        , no_argument       , &logs, 1 }
    , { "logcat"                      , no_argument       , &logs, 1 }
    , { "logcats"                     , no_argument       , &logs, 1 }
    , { "logexpr"                     , no_argument       , &logs, 1 }
    , { "logexprs"                    , no_argument       , &logs, 1 }

#if DEBUG || DEVEL
    , { "backtrace-pithy"             , no_argument       , &g_args.mode_backtrace, MODE_BACKTRACE_PITHY }
    , { "backtrace-full"              , no_argument       , &g_args.mode_backtrace, MODE_BACKTRACE_FULL }
#endif
    , { }
  };

  char * switches =
    // getopt options
    "-"

    // no-argument switches
    ""

    // with-argument switches
    ""
  ;

  //
  // args:defaults
  //
#if DEBUG || DEVEL
  g_args.mode_backtrace   = DEFAULT_MODE_BACKTRACE;
#endif

  int x;
  int indexptr;
  opterr = 0;
  while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], switches, longopts, &indexptr)) != -1)
  {
    if(x == 0)
    {
      // longopts - placeholder
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

  if(help || version || logs)
  {
    fatal(usage, 1, 1, help, logs);
  }

  finally : coda;
}

xapi args_summarize()
{
  enter;

  char space[512];
  int x;

  logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

  // log execution parameters under PARAMS
  logf(L_PARAMS , "%11sprocessors             =%ld"           , ""  , g_args.procs);

#if DEBUG || DEVEL
  logf(L_ARGS | L_PARAMS        , " %s (  %c  ) mode-backtrace         =%s", g_args.mode_backtrace == DEFAULT_MODE_BACKTRACE ? " " : "*", ' ', MODE_STR(g_args.mode_backtrace));
#endif

  if(g_args.concurrency == 0)
    snprintf(space, sizeof(space)  , "%s", "unbounded");
  else
    snprintf(space, sizeof(space)  , "%d", g_args.concurrency);

  logf(L_ARGS | L_PARAMS       , " %s (  %c  ) concurrency            =%s", g_args.concurrency == (int)((float)g_args.procs * 1.2f) ? " " : "*", 'j', space);

  if(g_args.objectsl == 0)
    logf(L_ARGS | L_PARAMS      , " %s (  %c  ) object(s)         =", " ", ' ');
  for(x = 0; x < g_args.objectsl; x++)
    logf(L_ARGS | L_PARAMS      , " %s (  %c  ) object(s)         =%s", "*", ' ', g_args.objects[x]);

  logs(L_ARGS | L_PARAMS, "--------------------------------------------------------------------------------");

  finally : coda;
}

void args_teardown()
{
  int x;
  for(x = 0; x < g_args.objectsl; x++)
    wfree(g_args.objects[x]);

  wfree(g_args.objects);
}
