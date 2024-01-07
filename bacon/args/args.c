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

static void args_version()
{
  dprintf(1, "bacon - bam configuration object notation %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void args_usage()
{
  dprintf(1,
"bacon - bam configuration object notation %s\n"
"\n"
"usage : bacon [options] [argument..]\n"
"\n"
"read bacon on stdin, possibly apply an operation, print bacon on stdout\n"
"\n"
"global options\n"
" --help | -h          print this message, exit\n"
" --version | -V       print version information, exit\n"
" -t         (default) argument is literal text\n"
" -f                   argument is path to a file\n"
" -s         (default) parse scalars in set context\n"
" -l                   parse scalars in list context\n"
" -m         (default) argument is bacon text to be merged onto stdin\n"
" -e                   argument is an expression to eval against stdin\n"
"\n"
"build optimally      https://git.sr.ht/~hossbeast/bam\n"
    , git_describe
  );

  exit(0);
}

static void process_nonoption(const char * const restrict s)
{
  if(g_args.arg) {
    dprintf(2, "unexpected extra arg : %s\n", s);
    exit(1);
  }
  g_args.arg = s;
}

void args_parse(int argc, char ** argv)
{
  int help = 0;
  int version = 0;

  struct option longopts[] = {
      { "help"      , no_argument , &help, 1 }
    , { "version"   , no_argument , &version, 1 }
    , { }
  };

  char * switches =
    // getopt options
    "-"

    // no-argument switches
    "hVftslem"
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
      g_args.input = IN_FILE;
    }
    else if(x == 't')
    {
      g_args.input = IN_TEXT;
    }
    else if(x == 'e')
    {
      g_args.oper = OP_EVAL;
    }
    else if(x == 'm')
    {
      g_args.oper = OP_MERGE;
    }
    else if(x == 's')
    {
      g_args.container = BACON_TYPE_SET;
    }
    else if(x == 'l')
    {
      g_args.container = BACON_TYPE_LIST;
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
        dprintf(2, "unknown switch -%c\n", optopt);
        exit(1);
      }
      else
      {
        dprintf(2, "unknown argument -%s\n", argv[optind-1]);
        exit(1);
      }
    }
    else if(optarg[0] == '+');
    {
      // non-option argv elements
      process_nonoption(optarg);
    }
  }

  for(; optind < argc; optind++)
  {
    // options following --
    process_nonoption(argv[optind]);
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
}
