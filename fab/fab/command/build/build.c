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
#include <signal.h>

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "narrator.h"
#include "narrator/units.h"
#include "valyria/list.h"
#include "value/writer.h"
#include "logger/arguments.h"

#include "build.h"
#include "command.h"
#include "errtab/MAIN.errtab.h"
#include "fab/client.h"

#include "macros.h"

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
"usage : fab build|autobuild [ <selector> ] ...\n"
"\n"
  );

  finally : coda;
}

static xapi args_parse(const char ** restrict argv, size_t argc)
{
  enter;

  int x;
  char * s = 0;
  char mode = 't';    // transitive by default
  char nextmode = 0;

  fatal(list_createx, &args.targets, 0, 0, wfree, 0);

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
    else if(x == 't')
    {
      nextmode = x;
    }
    else if(x == 'x')
    {
      nextmode = x;
    }
    else
    {
      if(optarg[0] == '-' && optarg[1] == 't')
        nextmode = 't';
      else if(optarg[0] == '+' && optarg[1] == 't')
        mode = 't';
      else if(optarg[0] == '-' && optarg[1] == 'x')
        nextmode = 'x';
      else if(optarg[0] == '+' && optarg[1] == 'x')
        mode = 'x';
      else
      {
        if(nextmode)
          fatal(ixstrncat, &s, &nextmode, 1);
        else
          fatal(ixstrncat, &s, &mode, 1);
        fatal(ixstrcat, &s, optarg);
        fatal(list_push, args.targets, s, 0);
        s = 0;
        nextmode = 0;
      }
    }
  }

  // options following --
  for(; optind < argc; optind++)
  {
    fatal(ixstrncat, &s, &mode, 1);
    fatal(ixstrcat, &s, optarg);
    fatal(ixstrdup, &s, argv[optind]);
    fatal(list_push, args.targets, s, 0);
    s = 0;
  }

finally:
  wfree(s);
coda;
}

static xapi command_say(narrator * N)
{
  enter;

  xsays(" build");

  if(args.targets->size)
    xsays(" --");

  int x;
  for(x = 0; x < args.targets->size; x++)
    xsayf(" %s", (char*)list_get(args.targets, x));

  finally : coda;
}

static xapi collate(value_writer * restrict writer, const char * restrict run)
{
  enter;

  int x;
  char *arg;

  fatal(narrator_xsays, writer->N, ""
"stage-config : {"
  );

  if(args.targets->size)
  {
    fatal(narrator_xsays, writer->N, ""
"formula : {"
" success : {"
"   show-path : true"
"   show-command : true"
"   show-cwd : true"
"   show-arguments : true"
"   show-environment : true"
"   show-sources : true"
"   show-targets : true"
"   show-status : true"
"   show-stdout : true"
" }"
" error : {"
"   show-path : true"
"   show-command : true"
"   show-cwd : true"
"   show-arguments : true"
"   show-environment : true"
"   show-sources : true"
"   show-targets : true"
"   show-status : true"
"   show-stdout : true"
" }"
"}"
    );
  }

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

  /* transitive targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 't')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, writer->N, ""
" select : ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 't')
        continue;

      fatal(narrator_xsayf, writer->N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, writer->N, ""
" ] "
" invalidate "
    );
  }

  /* direct targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 'x')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, writer->N, ""
" select : ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 'x')
        continue;

      fatal(narrator_xsayf, writer->N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, writer->N, ""
" ] "
"invalidate "
    );
  }

  fatal(narrator_xsays, writer->N, ""
" goals : {"
  );

  /* transitive targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 't')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, writer->N, ""
" target-transitive : ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 't')
        continue;

      fatal(narrator_xsayf, writer->N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, writer->N, ""
" ] "
    );
  }

  /* direct targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 'x')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, writer->N, ""
" target-direct: ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 't')
        continue;

      fatal(narrator_xsayf, writer->N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, writer->N, ""
" ] "
    );
  }

  fatal(narrator_xsayf, writer->N, ""
" build } %s", run
  );

  finally : coda;
}

//
// build
//

static xapi build_usage_say(narrator * restrict N)
{
  enter;

  fatal(usage_say, N);

  finally : coda;
}

static xapi build_args_parse(const char ** restrict argv, size_t argc)
{
  enter;

  fatal(args_parse, argv, argc);

  finally : coda;
}

static xapi build_command_say(narrator * N)
{
  enter;

  fatal(command_say, N);

  finally : coda;
}

static xapi build_collate(value_writer * restrict writer)
{
  enter;

  fatal(collate, writer, "run");

  finally : coda;
}

struct command * build_command = (struct command[]) {{
    args_parse : build_args_parse
  , usage_say : build_usage_say
  , command_say : build_command_say
  , collate : build_collate
}};

//
// autobuild
//

static xapi autobuild_usage_say(narrator * restrict N)
{
  enter;

  fatal(usage_say, N);

  finally : coda;
}

static xapi autobuild_args_parse(const char ** restrict argv, size_t argc)
{
  enter;

  fatal(args_parse, argv, argc);

  finally : coda;
}

static xapi autobuild_command_say(narrator * N)
{
  enter;

  fatal(command_say, N);

  finally : coda;
}

static xapi autobuild_collate(value_writer * restrict writer)
{
  enter;

  fatal(collate, writer, "autorun");

  finally : coda;
}

static xapi autobuild_conclusion()
{
  enter;

  sigset_t sigs;
  sigemptyset(&sigs);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

  sleep(1000000);

  finally : coda;
}

struct command * autobuild_command = (struct command[]) {{
    args_parse : autobuild_args_parse
  , usage_say : autobuild_usage_say
  , command_say : autobuild_command_say
  , collate : autobuild_collate
  , conclusion : autobuild_conclusion
}};

//
// public
//

xapi build_command_cleanup()
{
  enter;

  fatal(list_xfree, args.targets);

  finally : coda;
}
