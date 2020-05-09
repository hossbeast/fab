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
#include "valyria/list.h"
#include "value/writer.h"
#include "logger/arguments.h"

#include "invalidate.h"
#include "command.h"
#include "MAIN.errtab.h"

static struct {
  list * targets;
} args;

//
// static
//

static xapi usage_say(narrator * restrict N)
{
  enter;

  xsays(
"\n"
"usage : fab invalidate [ <selector>... ] ...\n"
"\n"
  );

  finally : coda;
}

static xapi args_parse(const char ** restrict argv, size_t argc)
{
  enter;

  int x;

  fatal(list_create, &args.targets);

  struct option longopts[] = {
    { }
  };

  // disable getopt error messages
  opterr = 0;

  int longindex;
  while((x = getopt_long(argc, (void*)argv, "-Bftx", longopts, &longindex)) != -1)
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
    fatal(list_push, args.targets, (void*)argv[optind], 0);
  }

  finally : coda;
}

static xapi command_say(narrator * N)
{
  enter;

  xsays(" invalidate");

  if(args.targets->size)
    xsays(" --");

  int x;
  for(x = 0; x < args.targets->size; x++)
    xsayf(" %s", (char*)list_get(args.targets, x));

  finally : coda;
}

//
// build
//

static xapi invalidate_usage_say(narrator * restrict N)
{
  enter;

  fatal(usage_say, N);

  finally : coda;
}

static xapi invalidate_args_parse(const char ** restrict argv, size_t argc)
{
  enter;

  fatal(args_parse, argv, argc);

  finally : coda;
}

static xapi invalidate_command_say(narrator * N)
{
  enter;

  fatal(command_say, N);

  finally : coda;
}

static xapi invalidate_collate(value_writer * restrict writer)
{
  enter;

  int x;
  char *arg;

  fatal(narrator_xsays, writer->N, ""
"stage-config : {"
  );

  fatal(narrator_xsays, writer->N, ""
"logging : {"
" console : {"
"   exprs : ["
  );

  fatal(narrator_xsays, writer->N, ""
"   \"+ERROR +WARN +INFO +BUILD\""
"   \"%CATEGORY\""
#if DEVEL
"   \"%NAMES %PID %TID\""
#endif
  );

  fatal(narrator_xsayf, writer->N, "\"%s\"", g_logvs);
  fatal(narrator_xsayf, writer->N, "\"%s\"", g_ulogvs);
  fatal(narrator_xsays, writer->N, ""
" ]"
"}"
  );

  fatal(narrator_xsays, writer->N, ""
"}} reconfigure "
  );

  fatal(narrator_xsays, writer->N, ""
" select : ["
  );

  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    fatal(narrator_xsayf, writer->N, ""
" pattern : %s", arg
    );
  }

  fatal(narrator_xsays, writer->N, ""
" ] "
" invalidate "
  );

  finally : coda;
}

struct command * invalidate_command = (struct command[]) {{
    args_parse : invalidate_args_parse
  , usage_say : invalidate_usage_say
  , command_say : invalidate_command_say
  , collate : invalidate_collate
}};

//
// public
//

xapi invalidate_command_cleanup()
{
  enter;

  fatal(list_xfree, args.targets);

  finally : coda;
}
