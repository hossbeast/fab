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

#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "xapi.h"
#include "fab/load.h"
#include "listwise/load.h"
#include "logger/load.h"
#include "lorien/load.h"
#include "narrator/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "yyutil/load.h"
#include "value/load.h"

#include "fab/ipc.h"
#include "fab/identity.h"
#include "fab/request.h"
#include "fab/response.h"
#include "fab/sigbank.h"
#include "listwise.h"
#include "listwise/LISTWISE.errtab.h"
#include "listwise/PCRE.errtab.h"
#include "listwise/lstack.h"
#include "listwise/object.h"
#include "listwise/operators.h"
#include "logger.h"
#include "logger/filter.h"
#include "logger/stream.h"
#include "logger/arguments.h"
#include "lorien/touch.h"
#include "narrator.h"
#include "xapi/SYS.errtab.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xprctl.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"

#include "config.h"
#include "errtab/CONFIG.errtab.h"
#include "logging.h"
#include "logging.h"
#include "server.h"
#include "tmp.h"
#include "usage.h"
#include "filesystem.h"

#include "cksum.h"
#include "macros.h"
#include "memblk.h"
#include "params.h"
#include "parseint.h"

static pid_t fab_pids[3];      // most recent fab pids

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R = 0;
  char space[1024];
  size_t tracesz = 0;
  int token = 0;
  int fd = -1;
  uint32_t hash = 0;

  fab_server * server = 0;
  memblk * mb = 0;

  // libraries
  fatal(fab_load);
  fatal(listwise_load);
  fatal(logger_load);
  fatal(lorien_load);
  fatal(narrator_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(yyutil_load);
  fatal(value_load);

  // modules
  fatal(sigbank_setup, "fabd");
  fatal(filesystem_setup);
  fatal(config_setup);
  fatal(logger_arguments_setup, envp);

  // locals
  fatal(memblk_mk, &mb);

#if 0
  // register object types with liblistwise
  fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
  fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

  // load additional fab-specific listwise operators
  fatal(listwise_register_opdir, XQUOTE(FABLWOPDIR));
#endif

  // fabd is normally invoked with a single argument, but in devel mode, check for nodaemon
  int daemon = 1;

#if DEVEL
  int x;
  for(x = 2; x < g_argc; x++)
  {
    if(strcmp(g_argv[x], "--nodaemon") == 0 || strcmp(g_argv[x], "--no-daemon") == 0)
    {
      daemon = 0;
      break;
    }
  }
#endif

  // allow creation of world+rw files
  // close standard file descriptors
  umask(0);
  fatal(xclose, 0);
  fatal(xopens, 0, O_RDONLY, "/dev/null");

  // get ipc dir
  if(argc < 2 || parseuint(g_argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &hash, 0) != 0)
    fail(SYS_BADARGS);

  // core file goes in cwd
  fatal(params_setup, hash);
  fatal(xchdirf, "%s/fabd", g_params.ipcdir);

  // logging, with per-ipc logfiles
  fatal(logging_setup, hash);
  fatal(logger_finalize);

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  fatal(params_report);

  // in non-daemon mode, the fabd lock will not have been inherited
  if(!daemon)
  {
    pid_t pgid = 0;
    fatal(ipc_lock_obtain, &pgid, "%s/fabd/lock", g_params.ipcdir);
    while(pgid)
    {
      fatal(ipc_lock_release, "%s/fabd/lock", g_params.ipcdir);
      fatal(ipc_lock_obtain, &pgid, "%s/fabd/lock", g_params.ipcdir);
    }
  }

  fatal(fab_server_create, &server);

  // load the initial config
  fatal(config_files_apply);

#if DEVEL
  #if __linux__
  // arrange to terminate if the parent fabw process dies
  fatal(xprctl, PR_SET_PDEATHSIG, SIGTERM, 0, 0, 0);
  #endif
#endif

  // prepare to receive requests
  fatal(identity_assume_fabsys);

  if(daemon)
    fatal(fab_server_ready, server);

  fab_request * request;
  do
  {
    fatal(fab_server_receive, server, daemon, &request);

    if(request)
    {
      fab_response * response = 0;
      fatal(fab_server_dispatch, server, request, mb, &response);

      // task complete
      fatal(fab_server_respond, server, mb, response);
    }

    fatal(usage_report);

    // cleanup tmp dir, including specifically the last fab pid we are tracking
    fatal(tmp_cleanup, &fab_pids[sizeof(fab_pids) / sizeof(fab_pids[0]) - 1], 1);

    // cycle fab pids
    memmove(&fab_pids[1], &fab_pids[0], sizeof(*fab_pids) * ((sizeof(fab_pids) / sizeof(fab_pids[0])) - 1));
    fab_pids[0] = server->client_pid;
  } while(request);

  // update stamp file
  fatal(touchf, FABIPC_MODE_DATA, XQUOTE(FABTMPDIR) "/pid/%d/stamp", g_params.pid);

  // cycle in my own pid
  memmove(&fab_pids[1], &fab_pids[0], sizeof(*fab_pids) * ((sizeof(fab_pids) / sizeof(fab_pids[0])) - 1));
  fab_pids[0] = g_params.pid;

  // cleanup tmp dir, including specifically all of the fab pids we are tracking and my own pid
  fatal(tmp_cleanup, fab_pids, sizeof(fab_pids) / sizeof(fab_pids[0]));

finally:
  fatal(log_finish, &token);

#if DEBUG || DEVEL
  if(log_would(L_IPC))
  {
    xapi_infos("name", "fabd");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("hash", "%x", hash);
  }
#endif

  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    tracesz = xapi_trace_full(space, sizeof(space));
#else
    tracesz = xapi_trace_pithy(space, sizeof(space));
#endif

    logf(L_ERROR, "\n%.*s", (int)tracesz, space);
  }

  // locals
  fatal(xclose, fd);
  fatal(fab_server_xfree, server);
  fatal(memblk_free, mb);

  // modules
  sigbank_teardown();
  fatal(params_cleanup);
  fatal(config_cleanup);
  fatal(filesystem_cleanup);

  // libraries
  fatal(fab_unload);
  fatal(listwise_unload);
  fatal(logger_unload);
  fatal(lorien_unload);
  fatal(narrator_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
  fatal(yyutil_unload);
  fatal(value_unload);

conclude(&R);
  xapi_teardown();

  return !!R;
}
