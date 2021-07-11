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

#include "xapi.h"
#include "logger/arguments.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "args.h"
#include "MAIN.errtab.h"

#include "macros.h"
#include "git-state.h"

struct g_args g_args;

static void args_version()
{
  printf("fabc - fab commander %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

//
// public
//

void args_usage()
{
  printf(
"fabc - fab commander %s\n"
"\n"
"usage: fabc [path | option]..\n"
"\n"
"options\n"
" --help | -h       print this message\n"
" --version | -V    print version information\n"
"\n"
"build optimally      https://github.com/hossbeast/fab\n"
    , git_describe
  );

  exit(0);
}

xapi args_parse()
{
  enter;

  int help = 0;
  int version = 0;
  int x;
  int indexptr;
  const char *p;

  struct option longopts[] = {
      { "help"     , no_argument , &help, 1 }
    , { "version"  , no_argument , &version, 1 }
    , { }
  };

  p = "hV"
#if DEVEL
  "v"
#endif
  ;

  opterr = 0;
  while(indexptr = 0, (x = getopt_long(g_argc, &g_argv[0], p, longopts, &indexptr)) != -1)
  {
    if(x == 0)
    {
      // longopts - placeholder
    }
#if DEVEL
    else if(x == 'v')
    {
      g_args.verbose++;
    }
#endif
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
      free(g_args.path);
      fatal(ixstrdup, &g_args.path, optarg);
    }
  }

  for(; optind < g_argc; optind++)
  {
    // options following --
    free(g_args.path);
    fatal(ixstrdup, &g_args.path, g_argv[optind]);
  }

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
