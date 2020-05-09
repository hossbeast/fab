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
#include "value.h"
#include "value/writer.h"
#include "value/parser.h"

#include "adhoc.h"
#include "command.h"
#include "errtab/MAIN.errtab.h"
#include "fab/client.h"

#include "macros.h"

static struct {
  char * request;
} args;

xapi adhoc_command_cleanup()
{
  enter;

  wfree(args.request);

  finally : coda;
}

static xapi adhoc_usage_say(narrator * restrict N)
{
  enter;

  xsays(
"\n"
"usage : fab adhoc [request] ...\n"
"\n"
  );

  finally : coda;
}

static xapi adhoc_args_parse(const char ** restrict argv, size_t argc)
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

static xapi adhoc_command_say(narrator * N)
{
  enter;

  xsays(" adhoc");

  if(args.request)
    xsays(" --");

  xsayf(" %s", args.request);

  finally : coda;
}

static xapi adhoc_collate(value_writer * const restrict writer)
{
  enter;

  fatal(narrator_xsays, writer->N, args.request);

  finally : coda;
}

static xapi adhoc_process(const struct fab_message * restrict response)
{
  enter;

  printf("%s\n", response->text);

  finally : coda;
}

command * adhoc_command = (command[]) {{
    args_parse : adhoc_args_parse
  , usage_say : adhoc_usage_say
  , command_say : adhoc_command_say
  , collate : adhoc_collate
  , process : adhoc_process
}};
