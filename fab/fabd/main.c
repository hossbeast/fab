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
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xapi/errtab.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger.h"
#include "logger/arguments.h"

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
#include "ff.h"
#include "module.h"
#include "extern.h"

#include "identity.h"
#include "parseint.h"

static xapi xmain_exit;
static xapi xmain()
{
  enter;

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

    pid_t pgid;
    fatal(ipc_lock_obtain, &pgid, 0, "%s/fabd/lock", g_params.ipcdir);

    if(pgid)
      failf(FABD_DAEMONEXCL, "pgid", "%ld", (long)pgid);
  }
  #endif

  g_params.thread_monitor = gettid();

  // launch other threads
  fatal(sigutil_defaults);
  fatal(notify_thread_launch);
  fatal(server_thread_launch);
  fatal(sweeper_thread_launch);

  // become the monitor thread
  fatal(monitor_thread);

  finally : coda;
}

static xapi xmain_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_trace_pithy, L_ERROR, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif

    xmain_exit = XAPI_ERRVAL;
    xapi_calltree_unwind();
  }
coda;
}

static xapi xmain_load(char ** envp)
{
  enter;

  uint32_t hash = 0;

  // libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(lorien_load);
  fatal(moria_load);
  fatal(narrator_load);
  fatal(value_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(yyutil_load);

  // modules
  fatal(config_setup);
  fatal(extern_setup);
  fatal(ff_setup);
  fatal(filesystem_setup);
  fatal(identity_setup);
  fatal(module_setup);
  fatal(node_setup);
  fatal(notify_thread_setup);
  fatal(sweeper_thread_setup);

  // initialize logger, main program
  fatal(logger_arguments_setup, envp);

  // get ipc dir
  if(g_argc < 2 || parseuint(g_argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &hash, 0) != 0)
    fail(SYS_BADARGS);

  // core file goes in cwd
  fatal(params_setup, hash);
  fatal(xchdirf, "%s/fabd", g_params.ipcdir);

  // logging with per-ipc logfiles
  fatal(logging_setup, hash);
  fatal(xmain_jump);

finally:
  // modules
  fatal(config_cleanup);
  fatal(extern_cleanup);
  fatal(filesystem_cleanup);
  fatal(module_cleanup);
  fatal(node_cleanup);
  fatal(notify_thread_cleanup);
  fatal(params_cleanup);
  fatal(reconfigure_cleanup);
  fatal(sweeper_thread_cleanup);
  fatal(ff_cleanup);
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
    // failures which cannot be logged with liblogger to stderr
    xapi_backtrace();
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;

  return !!R;
}
