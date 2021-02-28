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

#include <string.h>
#include <getopt.h>

#include "xapi.h"
#include "errtab/MAIN.errtab.h"
#include "xlinux/xstdlib.h"
#include "logger.h"
#include "logger/arguments.h"
#include "value.h"
#include "logger/expr.h"

#include "args.h"
#include "logging.h"

#include "macros.h"
#include "common/assure.h"

struct g_args_t g_args;
static size_t inputs_alloc;

static xapi usage(int valid, int version, int help, int logs)
{
  enter;

  printf(
"value : read/write/merge configuration text\n"
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
"usage : value [ [ option ] [ logexpr ] [ /path/to/configuration ] ] ...\n"
"\n"
" --help    : this message\n"
" --version : version information\n"
" --logs    : logger configuration\n"
"\n"
"----------------- [ options ] --------------------------------------------------------------------\n"
"\n"
" --nofork  : don't fork before each test\n"
"\n"
  );
}

if(logs)
{
printf(
"\n"
"----------------- [ logs ] -----------------------------------------------------------------------\n"
"\n"
);

  fatal(logger_expr_push, 0, "+LOGGER");
  fatal(logger_categories_report);
  fatal(logger_expr_pop, 0);
}

printf(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
);

exit(!valid);

  finally : coda;
}

static xapi process_nonoption(input_type type, value_type container, char * const restrict s)
{
  enter;

  fatal(assure, &g_args.inputs, sizeof(*g_args.inputs), g_args.inputsl, &inputs_alloc);
  g_args.inputs[g_args.inputsl].s = s;
  g_args.inputs[g_args.inputsl].type = type;
  g_args.inputs[g_args.inputsl].container = container;
  g_args.inputsl++;

  finally : coda;
}

xapi args_parse()
{
  enter;

  int help = 0;
  int version = 0;
  int logs = 0;
  input_type input = INPUT_TEXT;
  value_type container = 0;

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
    "h"

    // with-argument switches
    "f:t:s:l:"
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
    else if(x == 'f')
    {
      fatal(process_nonoption, INPUT_FILE, container, optarg);
    }
    else if(x == 't')
    {
      fatal(process_nonoption, INPUT_TEXT, container, optarg);
    }
    else if(x == 's')
    {
      fatal(process_nonoption, input, VALUE_TYPE_SET, optarg);
    }
    else if(x == 'l')
    {
      fatal(process_nonoption, input, VALUE_TYPE_LIST, optarg);
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
    else if(optarg[0] == '+')
    {
      if(strcmp(optarg + 1, "f") == 0)
      {
        input = INPUT_FILE;
      }
      else if(strcmp(optarg + 1, "t") == 0)
      {
        input = INPUT_TEXT;
      }
      else if(strcmp(optarg + 1, "l") == 0)
      {
        container = VALUE_TYPE_LIST;
      }
      else if(strcmp(optarg + 1, "s") == 0)
      {
        container = VALUE_TYPE_SET;
      }
      else
      {
        failf(MAIN_BADARGS, "unknown argument", "%s", optarg);
      }
    }
    else
    {
      // non-option argv elements
      fatal(process_nonoption, input, container, optarg);
    }
  }

  for(; optind < g_argc; optind++)
  {
    // options following --
    fatal(process_nonoption, input, container, g_argv[optind]);
  }

  if(g_args.inputsl == 0)
  {
    fatal(process_nonoption, INPUT_FILE, container, "/dev/stdin");
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

  int x;

  logs(L_ARGS, "--------------------------------------------------------------------------------");

  if(g_args.inputsl == 0)
    logf(L_ARGS, " %s (  %c  ) input(s)", " ", ' ');
  for(x = 0; x < g_args.inputsl; x++)
    logf(L_ARGS, " %s (  %c  ) input(s) %16s %s", "*", ' ', "", g_args.inputs[x].s);

  logs(L_ARGS, "--------------------------------------------------------------------------------");

  finally : coda;
}

void args_teardown()
{
  wfree(g_args.inputs);
}
