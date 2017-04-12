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
#include <sys/stat.h>
#include <sys/types.h>

#include "xapi.h"
#include "fab/load.h"
#include "logger/load.h"
#include "lorien/load.h"
#include "moria/load.h"
#include "narrator/load.h"
#include "value/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "yyutil/load.h"

#include "xapi/SYS.errtab.h"
#include "xapi/trace.h"
#include "xapi/errtab.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xinotify.h"
#include "xlinux/xsignal.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger.h"
#include "logger/arguments.h"
#include "logger/config.h"
#include "moria/graph.h"

#include "FABD.errtab.h"
#include "config.h"
#include "filesystem.h"
#include "logging.h"
#include "monitor_thread.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "reconfigure.h"
#include "server_thread.h"
#include "sweeper_thread.h"

#include "identity.h"
#include "parseint.h"

#if DEBUG || DEVEL
static xapi obtain_lock()
{
  enter;

  while(1)
  {
    pid_t pgid = 0;
    fatal(ipc_lock_obtain, &pgid, "%s/fabd/lock", g_params.ipcdir);

    // lock obtained
    if(pgid == 0)
      break;

    if(pgid == -1)
      continue;

    // check whether the lock holder is still running
    int r = 0;
    fatal(uxkill, -pgid, 0, &r);

    // already running
    if(r == 0)
      fail(FABD_EXCL);

    // not running ; forcibly release the lock, try again
    fatal(ipc_lock_release, "%s/fabd/lock", g_params.ipcdir);
  }
  
  finally : coda;
}
#endif

static xapi begin()
{
  enter;

  uint32_t hash = 0;

  fatal(sigutil_defaults);

#if DEBUG || DEVEL
  // this check is omitted in DEBUG/DEVEL mode because valgrind requires non-setgid and non-setuid executables
#else
  // this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions
  if(strcmp(g_euid_name, "fabsys") || strcmp(g_egid_name, "fabsys"))
  {
    xapi_info_pushf("real", "r:%s/%d:%s/%d", g_ruid_name, g_ruid, g_rgid_name, g_rgid);
    xapi_info_pushf("effective", "e:%s/%d:%s/%d", g_euid_name, g_euid, g_egid_name, g_egid);
    fail(FABD_EXEPERMS);
  }
#endif

  // get ipc dir
  if(g_argc < 2 || parseuint(g_argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &hash, 0) != 0)
    fail(SYS_BADARGS);

  // core file goes in cwd
  fatal(params_setup, hash);
  fatal(xchdirf, "%s/fabd", g_params.ipcdir);

  // logging, with per-ipc logfiles
  fatal(logging_setup, hash);
  logger_set_process_name("fabd");
  logger_set_process_categories(L_FABD);

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

  fatal(params_report);

  // allow creation of world+rw files
  umask(0);

  // close standard file descriptors
  fatal(xclose, 0);
  fatal(xopens, 0, O_RDONLY, "/dev/null");

#if DEBUG || DEVEL
  if(g_argc > 2 && strcmp(g_argv[2], "--nodaemon") == 0)
  {
    g_logging_skip_reconfigure = 1;
    g_server_no_acknowledge = 1;

    fatal(obtain_lock);
  }
#endif

  g_params.thread_monitor = gettid();
  fatal(notify_thread_launch);
  fatal(server_thread_launch);
  fatal(sweeper_thread_launch);
  fatal(monitor_thread);

  finally : coda;
}

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R;
  char space[4096];

  // libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(lorien_load);
  fatal(narrator_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(yyutil_load);
  fatal(value_load);
  fatal(moria_load);

  // modules
  fatal(config_setup);
  fatal(filesystem_setup);
  fatal(identity_setup);
  fatal(logger_arguments_setup, envp);
  fatal(node_setup);
  fatal(notify_thread_setup);
  fatal(sweeper_thread_setup);

  fatal(begin, argc, argv, envp);
finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());

    xapi_trace_full(space, sizeof(space), 0);
#else
    xapi_trace_pithy(space, sizeof(space), 0);
#endif

    xlogs(L_ERROR, L_NOCATEGORY, space);
  }

  // modules
  fatal(config_cleanup);
  fatal(filesystem_cleanup);
  fatal(node_cleanup);
  fatal(notify_thread_cleanup);
  fatal(params_cleanup);
  fatal(reconfigure_cleanup);
  fatal(sweeper_thread_cleanup);
  identity_teardown();

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(lorien_unload);
  fatal(moria_unload);
  fatal(narrator_unload);
  fatal(value_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
  fatal(yyutil_unload);
conclude(&R);
  xapi_teardown();

  return !!R;
}
