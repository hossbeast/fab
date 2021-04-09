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

#include <unistd.h>
#include <fcntl.h>

#include "xapi.h"
#include "fab/load.h"
#include "logger/load.h"
#include "narrator/load.h"
#include "value/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "yyutil/load.h"

#include "fab/client.h"
#include "fab/sigutil.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"

#include "args.h"
#include "MAIN.errtab.h"
#include "client_thread.h"
#include "logging.h"
#include "params.h"
#include "ui_thread.h"

__thread int32_t tid;

static xapi xmain_exit;
static xapi xmain()
{
  enter;

  sigset_t sigs;
  siginfo_t info;
  int r;
  int fd;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  fatal(args_parse);

#if DEVEL
  char space[512];
  snprintf(space, sizeof(space), "%s/../fabd/fabd.devel.xapi", g_params.exedir);
  g_fab_client_fabd_path = space;
#endif

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  // save stdin/stdout - ui thread fetches them from here
  fatal(xdup2, 0, TERMINAL_IN);
  fatal(xdup2, 1, TERMINAL_OUT);

  // discard stdout/stderr
#if DEVEL
  fd = open("/tmp/fabc", O_CREAT | O_WRONLY);
#else
  fd = open("/dev/null", O_WRONLY);
#endif
  fatal(xdup2, fd, 1);
  fatal(xdup2, fd, 2);

  fatal(sigutil_install_handlers);
  sigfillset(&sigs);
  fatal(xsigprocmask, SIG_SETMASK, &sigs, 0);

  fatal(client_thread_launch);
  fatal(ui_thread_launch);

  while(g_params.thread_count)
  {
    fatal(sigutil_wait, &sigs, &info);
    g_params.shutdown = true;
    fatal(uxtgkill, &r, g_params.pid, g_params.thread_client, SIGUSR1);
    fatal(uxtgkill, &r, g_params.pid, g_params.thread_ui, SIGUSR1);
  }

finally:
  if(XAPI_UNWINDING)
  {
    if(XAPI_ERRVAL == MAIN_BADARGS || XAPI_ERRVAL == MAIN_NOCOMMAND)
    {
      args_usage();
    }
  }
coda;
}

static xapi xmain_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fab");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());

    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE);
#else
    fatal(logger_trace_pithy, L_ERROR, XAPI_TRACE_COLORIZE);
#endif

    xmain_exit = XAPI_ERRVAL;
    xapi_calltree_unwind();
  }
coda;
}

static xapi xmain_load(char ** envp)
{
  enter;

  // load libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(narrator_load);
  fatal(value_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(yyutil_load);

  // load modules
  fatal(logging_setup, envp);
  fatal(params_setup);

  fatal(xmain_jump);

finally:
  // modules
  params_teardown();

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(value_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
  fatal(yyutil_unload);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  tid = g_params.thread_main = gettid();

  xapi R = 0;
  fatal(xmain_load, envp);

finally:
  if(XAPI_UNWINDING)
  {
    // write failures before liblogger to stderr
    xapi_backtrace(2, 0);
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;

  return !!R;
}
