/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <getopt.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"
#include "bacon/value.h"

#include "args.h"

#include "macros.h"
#include "common/assure.h"

#include "git-state.h"

struct g_args_t g_args;
static size_t inputs_alloc;

static void args_version()
{
  printf("bacon - bam configuration object notation %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void args_usage()
{
  printf(
"bacon - bam configuration object notation %s\n"
"\n"
"usage : bacon [option | path]..\n"
"\n"
"global options\n"
" --help | -h      print this message\n"
" --version | -V   print version information\n"
"\n"
"build optimally      https://git.sr.ht/~hossbeast/bam\n"
    , git_describe
  );

  exit(0);
}

static void process_nonoption(int input, int oper, bacon_type container, char * const restrict s)
{
  assure(&g_args.inputs, sizeof(*g_args.inputs), g_args.inputsl, &inputs_alloc);
  g_args.inputs[g_args.inputsl].s = s;
  g_args.inputs[g_args.inputsl].input = input;
  g_args.inputs[g_args.inputsl].oper = oper;
  g_args.inputs[g_args.inputsl].container = container;
  g_args.inputsl++;
}

void args_parse(int argc, char ** argv)
{
  int help = 0;
  int version = 0;
  int input = IN_TEXT;
  int oper = OP_MERGE;
  bacon_type container = BACON_TYPE_SET;

  struct option longopts[] = {
      { "help"      , no_argument , &help, 1 }
    , { "args"      , no_argument , &help, 1 }
    , { "params"    , no_argument , &help, 1 }
    , { "options"   , no_argument , &help, 1 }
    , { "opts"      , no_argument , &help, 1 }
    , { "version"   , no_argument , &version, 1 }
    , { "vers"      , no_argument , &version, 1 }
    , { "vrs"       , no_argument , &version, 1 }
    , { }
  };

  char * switches =
    // getopt options
    "-"

    // no-argument switches
    "hV"

    // with-argument switches
    "f:t:s:l:e:m:"
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
      process_nonoption(IN_FILE, oper, container, optarg);
    }
    else if(x == 't')
    {
      process_nonoption(IN_TEXT, oper, container, optarg);
    }
    else if(x == 'e')
    {
      process_nonoption(input, OP_EVAL, container, optarg);
    }
    else if(x == 'm')
    {
      process_nonoption(input, OP_MERGE, container, optarg);
    }
    else if(x == 's')
    {
      process_nonoption(input, oper, BACON_TYPE_SET, optarg);
    }
    else if(x == 'l')
    {
      process_nonoption(input, oper, BACON_TYPE_LIST, optarg);
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
        input = IN_FILE;
      }
      else if(strcmp(optarg + 1, "t") == 0)
      {
        input = IN_TEXT;
      }
      else if(strcmp(optarg + 1, "e") == 0)
      {
        oper = OP_EVAL;
      }
      else if(strcmp(optarg + 1, "m") == 0)
      {
        oper = OP_MERGE;
      }
      else if(strcmp(optarg + 1, "l") == 0)
      {
        container = BACON_TYPE_LIST;
      }
      else if(strcmp(optarg + 1, "s") == 0)
      {
        container = BACON_TYPE_SET;
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
      process_nonoption(input, oper, container, optarg);
    }
  }

  for(; optind < argc; optind++)
  {
    // options following --
    process_nonoption(input, oper, container, argv[optind]);
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
  free(g_args.inputs);
}
