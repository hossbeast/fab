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

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"

#include "args.h"

#include "macros.h"
#include "common/assure.h"
#include "git-state.h"

typeof(g_args) g_args;

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
"usage : xunit [ [ option ] [ logexpr ] ] ...\n"
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

void args_parse(int argc, char ** argv)
{
  int help = 0;
  int version = 0;
  int fork = 1;
  int x;
  int indexptr;

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

  opterr = 0;
  while(indexptr = 0, (x = getopt_long(argc, &argv[0], switches, longopts, &indexptr)) != -1)
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
        fprintf(stderr, "unknown switch -%c\n", optopt);
        exit(1);
      }
      else
      {
        fprintf(stderr, "unknown argument -%s\n", argv[optind-1]);
        exit(1);
      }
    }
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
}
