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
#include <sys/syscall.h>

#include "xapi.h"
#include "types.h"

#include "logger/load.h"
#include "value/load.h"
#include "xlinux/xstdlib.h"

#include "logger.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "value.h"
#include "value/parser.h"
#include "value/merge.h"
#include "narrator.h"

#include "args.h"
#include "MAIN.errtab.h"
#include "logging.h"

#include "common/snarf.h"

__thread int32_t tid;

static xapi xmain()
{
  enter;

  value_parser * parser = 0;
  char * text = 0;
  size_t text_len;
  value * result = 0;

  // parse cmdline arguments
  fatal(args_parse);
  fatal(args_summarize);

  fatal(value_parser_create, &parser);

  int x;
  for(x = 0; x < g_args.inputsl; x++)
  {
    const char * fname;
    char * s;
    size_t sl;
    if(g_args.inputs[x].type == INPUT_FILE)
    {
      fatal(snarfs, &text, &text_len, g_args.inputs[x].s);
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
    fatal(value_parser_parse, parser, s, sl, fname, g_args.inputs[x].container, &val);

    if(x == 0)
    {
      result = val;
    }
    else
    {
      fatal(value_merge, parser, &result, val, 0);
    }
  }

  fatal(value_say, result, g_narrator_stdout);
  fatal(narrator_xsays, g_narrator_stdout, "\n");

finally:
  wfree(text);
  fatal(value_parser_xfree, parser);
coda;
}

static xapi main_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE);
    xapi_calltree_unwind();
  }
coda;
}

static xapi main_load(char ** envp)
{
  enter;

  // load libraries
  fatal(logger_load);
  fatal(value_load);

  // load modules
  fatal(logging_setup, envp);

  fatal(main_jump);

finally:
  // modules
  args_teardown();

  // libraries
  fatal(logger_unload);
  fatal(value_unload);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  tid = syscall(SYS_gettid);

  xapi R = 0;
  fatal(main_load, envp);

finally:
  if(XAPI_UNWINDING)
  {
    // failures which cannot be logged with liblogger to stderr
    xapi_backtrace(2, 0);
  }

conclude(&R);
  xapi_teardown();

  return !!R;
}
