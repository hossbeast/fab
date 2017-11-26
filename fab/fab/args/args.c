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

#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/mempolicy.h"
#include "narrator.h"
#include "narrator/units.h"
#include "narrator/growing.h"
#include "fab/request.h"
#include "logger.h"
#include "logger/expr.h"
#include "logger/category.h"
#include "logger/arguments.h"
#include "valyria/list.h"

#include "args.h"
#include "logging.h"
#include "params.h"
#include "errtab/MAIN.errtab.h"
#include "build.h"

#include "macros.h"
#include "assure.h"
#include "memblk.h"

#define restrict __restrict

//
// public
//

xapi args_usage(const command * restrict cmd, int version, int logcats)
{
  enter;

  narrator * N = g_narrator_stdout;

  xsays(
"fab : build optimally\n"
  );

  if(version)
  {
    xsays(
" fab-" XQUOTE(FABVERSIONS)
#if DEVEL
"+DEVEL"
#elif DEBUG
"+DEBUG"
#endif
" @ " XQUOTE(BUILDSTAMP)
"\n"
    );
  }

  if(cmd)
    fatal(cmd->usage_say, N);

  if(logcats)
  {
    xsays(
"\n"
"----------------- [ logs ] -----------------------------------------------------------------------\n"
"\n"
    );

    fatal(logger_expr_push, 0, "+LOGGER");
    fatal(logger_categories_report);
    fatal(logger_expr_pop, 0);
  }

  xsays(
"\n"
"For more information visit http://fabutil.org\n"
"\n"
  );

  finally : coda;
}

xapi args_parse(const command ** cmd)
{
  enter;

  int x;
  const char ** argv = 0;
  size_t argc = 0;

  *cmd = 0;
  if(g_argc < 2)
    fail(MAIN_NOCOMMAND);

  // built-in subcommands
  else if(strcmp(g_argv[1], "build") == 0)
    *cmd = build_command;
  else
    fail(MAIN_NOCOMMAND);

  fatal(xmalloc, &argv, sizeof(*argv) * (g_argc - 1));

  argv[argc++] = g_argv[1];

  int help = 0;
  int version = 0;
  int logs = 0;
  for(x = 2; x < g_argc; x++)
  {
    if(strcmp(g_argv[x], "--") == 0)
      break;
    else if(strcmp(g_argv[x], "help") == 0)
      help = 1;
    else if(strcmp(g_argv[x], "args") == 0)
      help = 1;
    else if(strcmp(g_argv[x], "params") == 0)
      help = 1;
    else if(strcmp(g_argv[x], "options") == 0)
      help = 1;
    else if(strcmp(g_argv[x], "opts") == 0)
      help = 1;
    else if(strcmp(g_argv[x], "version") == 0)
      version = 1;
    else if(strcmp(g_argv[x], "logs") == 0)
      logs = 1;
    else
      goto add;
    continue;
    add : argv[argc++] = g_argv[x];
  }

  for(; x < g_argc; x++)
    g_argv[argc++] = g_argv[x];

  if(*cmd)
  {
    fatal((*cmd)->args_parse, argv, argc);
  }

  if(help || version || logs)
  {
    fatal(args_usage, *cmd, version, logs);
  }

finally:
  wfree(argv);
coda;
}

xapi args_report(const command * restrict cmd)
{
  enter;

  narrator * N = 0;

  fatal(narrator_growing_create, &N);

  xsays("fab");
  fatal(cmd->command_say, N);
  xsayf(" %s", g_logvs);

  logs(L_ARGS, narrator_growing_buffer(N));

finally:
  fatal(narrator_xfree, N);
coda;
}

xapi args_collate(const command * restrict cmd, memblk * restrict mb, fab_request ** restrict req)
{
  enter;

  int mpc = 0;
  int config_applied = 0;

  fatal(mempolicy_push, memblk_getpolicy(mb), &mpc);
  fatal(fab_request_create, req);
  fatal(fab_request_config_stages, *req, "logging.console.exprs = [ \"+WARN +INFO\" ]");
  if(g_logc)
    fatal(fab_request_config_stagef, *req, "logging.console.exprs += [ \"%s\" ]", g_logvs);
  if(g_ulogc)
    fatal(fab_request_config_stagef, *req, "logging.console.exprs += [ \"%s\" ]", g_ulogvs);

  fatal(cmd->collate, *req, &config_applied);

  if(!config_applied)
    fatal(fab_request_config_apply, *req);

finally:
  mempolicy_unwind(&mpc);
coda;
}
