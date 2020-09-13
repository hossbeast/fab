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

#include "xapi.h"
#include "xapi/calltree.h"
#include "fab/load.h"
#include "logger/load.h"
#include "narrator/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "value/load.h"

#include "xapi/trace.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstat.h"
#include "xlinux/xshm.h"
#include "xlinux/xpthread.h"
#include "narrator.h"
#include "logger.h"

#include "fab/sigutil.h"
#include "fab/client.h"
#include "fab/FAB.errtab.h"

#include "args.h"
#include "command.h"
#include "params.h"
#include "logging.h"
#include "build.h"
#include "MAIN.errtab.h"

#include "macros.h"

static xapi xmain_exit;
static xapi xmain()
{
  enter;

#if DEVEL
  char space[512];
#endif
  char * fabw_path = 0;
  const command * cmd = 0;

  fab_client * client = 0;

  void * request_shm = 0;
  void * response_shm = 0;

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  // parse cmdline arguments
  fatal(args_parse, &cmd);
  fatal(args_report, cmd);

  // ensure fabd can write to my stdout/stderr
  fatal(xfchmod, 1, 0777);
  fatal(xfchmod, 2, 0777);

#if DEVEL
  snprintf(space, sizeof(space), "%s/../fabw/fabw.devel.xapi", g_params.exedir);
  fabw_path = space;
#endif

  fatal(fab_client_create, &client, ".", XQUOTE(FABIPCDIR), fabw_path);

#if 0
  // kill the existing fabd instance, if any
  if(changed credentials)
    fatal(client_terminate);
#endif

  fatal(fab_client_prepare, client);
  fatal(fab_client_launchp, client);

  fatal(fab_client_prepare_request_shm, client, &request_shm);
  fatal(args_collate, cmd, request_shm);

  void * shmaddr = request_shm;
  request_shm = 0;
  fatal(fab_client_make_request, client, shmaddr, &response_shm);

  // consume the response
  shmaddr = response_shm;
  response_shm = 0;
  if(cmd->process) {
    fatal(cmd->process, shmaddr);
  }

  fatal(fab_client_release_response, client, shmaddr);
  fatal(fab_client_unlock, client);

  if(cmd->conclusion) {
    fatal(cmd->conclusion);
  }

finally:
  if(XAPI_UNWINDING)
  {
    if(XAPI_ERRVAL == FAB_NODAEMON)
    {
#if DEBUG || DEVEL
      // fabd exited - check for a coredump
#endif
    }
    else if(XAPI_ERRVAL == MAIN_BADARGS || XAPI_ERRVAL == MAIN_NOCOMMAND)
    {
      fatal(args_usage, cmd, 1, 1);
    }
  }

  // locals
  fatal(fab_client_xfree, client);

  fatal(xshmdt, request_shm);
  fatal(xshmdt, response_shm);

  // module teardown
  fatal(build_command_cleanup);
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
  fatal(logger_load);
  fatal(narrator_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(value_load);

  // load modules
  fatal(logging_setup, envp);
  fatal(params_setup);
  fatal(sigutil_defaults);

  // unblock SIGINT
  sigset_t cur;
  sigset_t sigs;
  sigemptyset(&sigs);
  fatal(xpthread_sigmask, SIG_BLOCK, &sigs, &cur);
  sigs = cur;
  sigdelset(&sigs, SIGINT);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

  fatal(xmain_jump);

finally:
  // modules
  params_teardown();

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
  fatal(value_unload);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

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
