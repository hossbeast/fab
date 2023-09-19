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
#include <stdio.h>

#include "xlinux/xstdlib.h"
#include "value.h"

#include "args.h"

#include "macros.h"
#include "common/assure.h"

#include "git-state.h"

struct g_args_t g_args;
static size_t inputs_alloc;

static void args_version()
{
  printf("fab %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void args_usage()
{
  printf(
"value : read/write/merge bacon text %s\n"
"\n"
"usage : value [ [ option ] [ logexpr ] [ /path/to/configuration ] ] ...\n"
" --help | -h      print this message\n"
" --version | -V   print version information\n"
" --nofork         don't fork before each test\n"
"\n"
"build optimally      https://github.com/hossbeast/fab\n"
    , git_describe
  );

  exit(0);
}

static void process_nonoption(input_type type, value_type container, char * const restrict s)
{
  assure(&g_args.inputs, sizeof(*g_args.inputs), g_args.inputsl, &inputs_alloc);
  g_args.inputs[g_args.inputsl].s = s;
  g_args.inputs[g_args.inputsl].type = type;
  g_args.inputs[g_args.inputsl].container = container;
  g_args.inputsl++;
}

void args_parse(int argc, char ** argv)
{
  int help = 0;
  int version = 0;
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
    , { }
  };

  char * switches =
    // getopt options
    "-"

    // no-argument switches
    "hV"

    // with-argument switches
    "f:t:s:l:"
  ;

  int x;
  int indexptr;
  opterr = 0;
  while(indexptr = 0, (x = getopt_long(argc, &argv[0], switches, longopts, &indexptr)) != -1)
  {
    if(x == 0)
    {
      // longopts - placeholder
    }
    else if(x == 'f')
    {
      process_nonoption(INPUT_FILE, container, optarg);
    }
    else if(x == 't')
    {
      process_nonoption(INPUT_TEXT, container, optarg);
    }
    else if(x == 's')
    {
      process_nonoption(input, VALUE_TYPE_SET, optarg);
    }
    else if(x == 'l')
    {
      process_nonoption(input, VALUE_TYPE_LIST, optarg);
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
        fprintf(stderr, "unknown argument %s", optarg);
        exit(1);
      }
    }
    else
    {
      // non-option argv elements
      process_nonoption(input, container, optarg);
    }
  }

  for(; optind < argc; optind++)
  {
    // options following --
    process_nonoption(input, container, argv[optind]);
  }

  if(g_args.inputsl == 0)
  {
    process_nonoption(INPUT_FILE, container, "/dev/stdin");
  }

  if(help)
  {
    args_usage();
  }
  if(version)
  {
    args_version();
  }
}

void args_teardown()
{
  wfree(g_args.inputs);
}
