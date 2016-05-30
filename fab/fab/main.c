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

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux.h"
#include "xlinux/SYS.errtab.h"
#include "narrator.h"
#include "narrator/units.h"
#include "logger.h"
#include "logger/arguments.h"
#include "lorien.h"
#include "lorien/canon.h"

#include "fabcore.h"
#include "fabcore/sigbank.h"
#include "fabcore/params.h"
#include "fabcore/identity.h"
#include "fabcore/path.h"
#include "fabcore/dirutil.h"
#include "fabcore/request.h"
#include "fabcore/ipc.h"

#include "global.h"
#include "args.h"
#include "logging.h"
#include "client.h"

#include "macros.h"
#include "memblk.h"
#include "memblk.def.h"
#include "parseint.h"
#include "cksum.h"

#define restrict __restrict

//
// public
//

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R = 0;
  int token = 0;
  char space[2048];
  char space2[2048];
  char hash[9] = { 0 };
  char init_fabfile_canon[512] = { 0 };
  char ipcdir[512];

  memblk * mb = 0;
  client * client = 0;
  request * req = 0;

  int fd = -1;
  size_t tracesz = 0;

  struct timespec time_start = {};
  struct timespec time_end = {};

  // libraries
  fatal(logger_load);
  fatal(xlinux_load);
  fatal(fabcore_load);
  fatal(narrator_load);
  fatal(lorien_load);

  // logging
  fatal(logging_setup);
  fatal(logger_initialize, envp);

  // modules
  fatal(sigbank_setup, "fab");

  // locals
  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_start);
  fatal(memblk_mk, &mb);

  // parse cmdline arguments into the block
  fatal(args_parse);
  fatal(args_report);

  // canonical path to the initial fabfile
  fatal(canon, "./fabfile", 0, g_params.cwd, g_params.cwdl, init_fabfile_canon, sizeof(init_fabfile_canon), 0, CAN_REALPATH);
  uint32_t u32 = cksum(init_fabfile_canon, sizeof(init_fabfile_canon));
  snprintf(hash, sizeof(hash), "%08x", u32);
  snprintf(ipcdir, sizeof(ipcdir), "%s/%s", XQUOTE(FABIPCDIR), hash);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fab");
  off_t off = 0;
  fatal(narrator_seek, log_narrator(), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s started", MAX(0, 20 - off), "");
  fatal(log_finish, &token);
#endif

  // client directory
  fatal(mkdirpf, FABIPC_DIR, "%s/client", ipcdir);

  // exclusive lock
  while(1)
  {
    pid_t pgid;
    fatal(ipc_lock_obtain, &pgid, "%s/client/lock", ipcdir);

    // lock obtained
    if(pgid == 0)
      break;

    int r;
    fatal(uxkill, -pgid, 0, &r);

    // already running
    if(r == 0)
      failf(FAB_FABLOCK, "pgid", "%ld", (long)pgid);

    // not running ; forcibly release the lock, try again
    fatal(ipc_lock_release, "%s/client/lock", ipcdir);
  }

  // canonical fabfile symlink
  snprintf(space, sizeof(space), "%s/fabfile", ipcdir);
  fatal(uxsymlink, init_fabfile_canon, space);

  // fab-cwd symlink
  snprintf(space, sizeof(space), "%s/client/cwd", ipcdir);
  fatal(uxunlink, space);
  fatal(xsymlink, g_params.cwd, space);

  // fab-out symlink
  snprintf(space2, sizeof(space2), "/proc/%ld/fd/1", (long)g_params.pid);
  ssize_t ss = 0;
  fatal(xreadlink, space2, space2, sizeof(space2), &ss);
  if(ss >= sizeof(space2))
    ss = sizeof(space2) - 1;
  if(ss >= 0)
    space2[ss] = 0;
  snprintf(space, sizeof(space), "%s/client/out", ipcdir);
  fatal(uxunlink, space);
  fatal(xsymlink, space2, space);

  // fab-err symlink
  snprintf(space2, sizeof(space2), "/proc/%ld/fd/2", (long)g_params.pid);
  ss = 0;
  fatal(xreadlink, space2, space2, sizeof(space2), &ss);
  if(ss >= sizeof(space2))
    ss = sizeof(space2) - 1;
  if(ss >= 0)
    space2[ss] = 0;
  snprintf(space, sizeof(space), "%s/client/err", ipcdir);
  fatal(uxunlink, space);
  fatal(xsymlink, space2, space);

  // fab-pid file
  snprintf(space, sizeof(space), "%s/client/pid", ipcdir);
  fatal(xopen_mode, space, O_CREAT | O_WRONLY, FABIPC_DATA, &fd);
  fatal(axwrite, fd, &g_params.pid, sizeof(g_params.pid));
  fatal(ixclose, &fd);

  // fabd directories
  fatal(mkdirpf, FABIPC_DIR, "%s/fabd", ipcdir);

  // ensure fabd can write to my stdout/stderr
  fatal(xfchmod, 1, 0777);
  fatal(xfchmod, 2, 0777);

  fatal(client_create, &client, ipcdir, hash);

#if 0
  // possibly kill the existing fabd instance, if any
  if(changed credentials)
    fatal(client_terminate);
#endif

  fatal(client_launchp, client);

  fatal(args_collate_request, mb, &req);
  fatal(client_make_request, client, mb, req);

finally:
  // stop the clock
  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &time_end);
  fatal(log_start, L_INFO, &token);
  logs(0, "elapsed : ");
  fatal(elapsed_say, &time_start, &time_end, log_narrator());
  fatal(log_finish, &token);

#if DEBUG || DEVEL
  xapi_infos("name", "fab");
  xapi_infof("pid", "%ld", (long)getpid());
  xapi_infos("hash", hash);
#endif

  // when failing due to an error propagated from fabd (fabd_error), do not log the
  // stacktrace, because fabd will have already done that
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    if(g_args && g_args->mode_backtrace == MODE_BACKTRACE_FULL)
      tracesz = xapi_trace_full(space, sizeof(space));
    else
#endif
      tracesz = xapi_trace_pithy(space, sizeof(space));

    xlogw(L_ERROR, L_RED, space, tracesz);
  }

  // locals
  fatal(ixclose, &fd);
  fatal(client_dispose, &client);
  memblk_free(mb);

  // module teardown
  sigbank_teardown();
  args_teardown();

  // libraries
  fatal(logger_unload);
  fatal(xlinux_unload);
  fatal(fabcore_unload);
  fatal(narrator_unload);
  fatal(lorien_unload);

conclude(&R);
  xapi_teardown();

  return !!R;
}
