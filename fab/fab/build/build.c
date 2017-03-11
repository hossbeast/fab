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
#include "fab/request.h"
#include "valyria/list.h"

#include "args.h"
#include "errtab/MAIN.errtab.h"
#include "build.h"

#define restrict __restrict

#define FORCE_ALL     1
#define FORCE_TARGETS 2

static struct {
  list * targets;
  int target_mode;
  int force_mode;
  int dryrun;
} args;

xapi build_command_cleanup()
{
  enter;

  fatal(list_xfree, args.targets);

  finally : coda;
}

static xapi build_usage_say(narrator * restrict N)
{
  enter;

  says(
"\n"
"usage : fab build [ [ <selector> ] [ <option> ] ] ...\n"
"\n"
"----------- [ options ] ------------------------------------------------------------------\n"
"\n"
" --target                      foo\n"
" --dryrun                      foo\n"
" --exact                       foo\n"
" --nofile                      foo\n"
" --force | -f                  foo\n"
" --all | -B                    foo\n"
  );

  finally : coda;
}

static xapi build_args_parse(const char ** restrict argv, size_t argc)
{
  enter;

  int x;
  char * s = 0;

  fatal(list_createx, &args.targets, wfree, 0, 0);

  struct option longopts[] = {
      { "target"        , no_argument , &args.target_mode, FAB_ATTR_TARGET }
    , { "exact"         , no_argument , &args.target_mode, FAB_ATTR_TARGET_EXACT }
    , { "nofile"        , no_argument , &args.target_mode, FAB_ATTR_TARGET_NOFILE }
    , { "dryrun"        , no_argument , &args.dryrun, 1 }
    , { "force"         , no_argument , &args.force_mode, FORCE_TARGETS }
    , { "all"           , no_argument , &args.force_mode, FORCE_ALL }
    , { }
  };

  // disable getopt error messages
  opterr = 0;

  int longindex;
  while((x = getopt_long(argc, (void*)argv, "-Bf", longopts, &longindex)) != -1)
  {
    if(x == 0)
    {
      // longopts
    }
    else if(x == 'f')
    {
      args.force_mode = FORCE_TARGETS;
    }
    else if(x == 'B')
    {
      args.force_mode = FORCE_ALL;
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
      fatal(ixstrdup, &s, optarg);
      fatal(list_push, args.targets, s);
      s = 0;
    }
  }

  // options following --
  for(; optind < argc; optind++)
  {
    fatal(ixstrdup, &s, argv[optind]);
    fatal(list_push, args.targets, s);
    s = 0;
  }

finally:
  wfree(s);
coda;
}

static xapi build_command_say(narrator * N)
{
  enter;

  says(" build");

  if(args.dryrun)
    says(" --dryrun");

  if(args.target_mode == FAB_ATTR_TARGET_EXACT)
    says(" --exact");
  else if(args.target_mode == FAB_ATTR_TARGET_NOFILE)
    says(" --nofile");
  else
    says(" --target");

  if(args.force_mode == FORCE_TARGETS)
    says(" --force");
  else if(args.force_mode == FORCE_ALL)
    says(" --all");

  if(args.targets->l)
    says(" --");

  int x;
  for(x = 0; x < args.targets->l; x++)
    sayf(" %s", (char*)list_get(args.targets, x));

  finally : coda;
}

static xapi build_collate(fab_request * const restrict req, int * restrict config_applied)
{
  enter;

  int x;

  fatal(fab_request_config_apply, req);
  *config_applied = 1;

  if(args.force_mode == FORCE_ALL)
  {
    fatal(fab_request_invalidate_all, req);
  }
  else if(args.force_mode == FORCE_TARGETS)
  {
    for(x = 0; x < args.targets->l; x++)
      fatal(fab_request_invalidate, req, list_get(args.targets, x));
  }

  uint32_t attr = FAB_ATTR_TARGET;
  if(args.target_mode)
    attr = args.target_mode;

  for(x = 0; x < args.targets->l; x++)
    fatal(fab_request_select, req, attr, list_get(args.targets, x));

  fatal(fab_request_build, req);

  finally : coda;
}

struct command * build_command = (struct command[]) {{
    args_parse : build_args_parse
  , usage_say : build_usage_say
  , command_say : build_command_say
  , collate : build_collate
}};
