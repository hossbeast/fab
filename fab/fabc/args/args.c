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

#include "args.h"
#include "logging.h"
#include "params.h"
#include "errtab/MAIN.errtab.h"

#include "macros.h"

struct g_args g_args;

//
// public
//

xapi args_usage(int version, int logcats)
{
  enter;

  narrator * N = g_narrator_stdout;

  xsays(
"fab : build optimally\n"
  );

  if(version)
  {
    xsays(
" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
"+DEVEL"
#elif DEBUG
"+DEBUG"
#endif
" @ " XQUOTE(BUILDSTAMP)
"\n"
    );
  }

  if(logcats)
  {
    xsays(
"\n"
"----------------- [ logs ] -----------------------------------------------------------------------\n"
"\n"
    );

    fatal(logger_expr_push, 0, "+LOGGER");
    fatal(logger_categories_report);
    fatal(logger_expr_pop, 0);
  }

  xsays(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
  );

  finally : coda;
}

xapi args_parse()
{
  enter;

  int help = 0;
  int version = 0;
  int logs = 0;

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
        failf(MAIN_BADARGS, "switch", "-%c", optopt);
      }
      else
      {
        failf(MAIN_BADARGS, "argument", "%s", g_argv[optind-1]);
      }
    }
    else
    {
      // non-option argv elements
      free(g_args.lookup);
      fatal(ixstrdup, &g_args.lookup, optarg);
    }
  }

  for(; optind < g_argc; optind++)
  {
    // options following --
    free(g_args.lookup);
    fatal(ixstrdup, &g_args.lookup, g_argv[optind]);
  }

  if(help || version || logs)
  {
    fatal(args_usage, version, logs);
  }

  finally : coda;
}

xapi args_report()
{
  enter;

  narrator * N = 0;

  fatal(narrator_growing_create, &N);

  xsays("fabc");
  xsayf(" %s", g_logvs);

  logs(L_ARGS, narrator_growing_buffer(N));

finally:
  fatal(narrator_xfree, N);
coda;
}
