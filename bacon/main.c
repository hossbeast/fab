/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "common/snarf.h"
#include "narrator.h"
#include "bacon/load.h"
#include "bacon/merge.h"
#include "bacon/parser.h"
#include "xlinux/xstdlib.h"

#include "args.h"

__thread int32_t tid;

int main(int argc, char ** argv, char ** envp)
{
  value_parser * parser = 0;
  char * text = 0;
  size_t text_len;
  value * result = 0;
  const char * fname;
  char * s;
  size_t sl;
  int x;

  tid = syscall(SYS_gettid);

  bacon_load();

  // parse cmdline arguments
  args_parse(argc, argv);

  value_parser_create(&parser);

  for(x = 0; x < g_args.inputsl; x++)
  {
    if(g_args.inputs[x].type == INPUT_FILE)
    {
      snarfs(&text, &text_len, g_args.inputs[x].s);
      s = text;
      sl = text_len;
      fname = g_args.inputs[x].s;
    }
    else
    {
      s = g_args.inputs[x].s;
      sl = strlen(s);
      fname = "-argv-";
    }

    value * val;
    value_parser_parse(parser, s, sl, fname, g_args.inputs[x].container, &val);

    if(x == 0)
    {
      result = val;
    }
    else
    {
      value_merge(parser, &result, val, 0);
    }
  }

  value_say(result, g_narrator_stdout);
  narrator_says(g_narrator_stdout, "\n");

  free(text);
  value_parser_xfree(parser);

  args_teardown();
  bacon_unload();
}
