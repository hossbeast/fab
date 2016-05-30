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
#include <sys/prctl.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux.h"
#include "narrator.h"
#include "valyria.h"
#include "logger.h"
#include "logger/filter.h"
#include "logger/stream.h"
#include "fabcore.h"
#include "lorien.h"
#include "yyutil.h"
#include "listwise.h"

#include "listwise/object.h"
#include "listwise/operators.h"
#include "listwise/lstack.h"
#include "listwise/LISTWISE.errtab.h"
#include "listwise/PCRE.errtab.h"

#include "fabcore/identity.h"
#include "fabcore/path.h"
#include "fabcore/params.h"
#include "fabcore/sigbank.h"
#include "fabcore/ipc.h"
#include "fabcore/request.h"
#include "fabcore/response.h"
#include "fabcore/logging.h"

#include "global.h"
#include "logging.h"
#include "server.h"
#include "usage.h"
#include "logging.h"

//#include "fabd_handler.h"

//#include "gn.h"
//#include "gnlw.h"
//#include "lwutil.h"
//#include "var.h"
//#include "ffproc.h"
//#include "cfg.h"
//#include "cfg.protected.h"
//#include "fml.h"
//#include "selector.h"
#include "tmp.h"
#include "server.h"

#include "parseint.h"
#include "macros.h"
#include "memblk.h"
#include "cksum.h"

static pid_t fab_pids[3];      // most recent fab pids

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R = 0;
  int token = 0;
  char space[2048];
  char space2[2048];
  char ipcdir[512];
  char hash[9] = { 0 };
  int fd = -1;
  size_t tracesz = 0;

  server * server = 0;
  memblk * mb = 0;

  // redirect stdout/stderr to the client
  snprintf(space, sizeof(space), "/dev/pts/20");
  fatal(xopen, space, O_RDWR, &fd);
  fatal(xdup2, fd, 1);
  fatal(ixclose, &fd);

  snprintf(space, sizeof(space), "/dev/pts/20");
  fatal(xopen, space, O_RDWR, &fd);
  fatal(xdup2, fd, 2);
  fatal(ixclose, &fd);

  // libraries
  fatal(xlinux_load);
  fatal(listwise_load);
  fatal(valyria_load);
  fatal(logger_load);
  fatal(narrator_load);
  fatal(fabcore_load);
  fatal(lorien_load);
  fatal(yyutil_load);

  // logging
  fatal(logging_setup);
  fatal(logger_initialize, envp);

  // modules
  fatal(sigbank_setup, "fabd");

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
  if(argc > 2)
    if(strcmp(argv[2], "--nodaemon") == 0)
      daemon = 0;
#endif

  // close standard file descriptors
  fatal(xclose, 0);
  fatal(xopen, "/dev/null", O_RDONLY, 0);

  // get ipc dir
  uint32_t u32;
  if(argc < 2 || parseuint(argv[1], SCNx32, 1, UINT32_MAX, 1, UINT8_MAX, &u32, 0) != 0)
    fail(FAB_BADARGS);
  snprintf(hash, sizeof(hash), "%08x", u32);
  snprintf(ipcdir, sizeof(ipcdir), "%s/%s", XQUOTE(FABIPCDIR), hash);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logf(0, "%ld/%ld/%s ", getpgid(0), getpid(), "fabd");
  off_t off = 0;
  fatal(narrator_seek, log_narrator(), 0, NARRATOR_SEEK_CUR, &off);
  logf(0, "%*s started", MAX(0, 20 - off), "");
  fatal(log_finish, &token);
#endif

  // exclusive lock
  pid_t pgid;
  fatal(ipc_lock_obtain, &pgid, "%s/fabd/lock", ipcdir);
  if(pgid && pgid != g_params.pgid)
    failf(FAB_FABDLOCK, "pgid", "%ld", (long)pgid);

  // allow creation of world-rw files
  umask(0);
  fatal(xchdir, "/");
  fatal(identity_assume_fabsys);

  // create symlink for dsc in hashdir
  snprintf(space, sizeof(space), "%s/dsc", ipcdir);
  snprintf(space2, sizeof(space2), XQUOTE(FABTMPDIR) "/pid/%d/dsc", g_params.pid);
  fatal(uxunlink, space);
  fatal(xsymlink, space2, space);

#if __linux__
  // arrange to terminate if the parent fabw process dies
  fatal(xprctl, PR_SET_PDEATHSIG, SIGTERM, 0, 0, 0);
#endif

  fatal(server_create, &server, ipcdir, hash);

  if(daemon)
    fatal(server_ready, server);

  while(1)
  {
    // receive a signal from fab
    int signum = 0;
    pid_t pid = 0;
    fatal(sigbank_receive, (int[]){ FABSIG_SYN, 0 }, 0, &signum, &pid);

    if(signum == SIGINT || signum == SIGQUIT || signum == SIGTERM)
      break;

    fatal(server_validate, server, pid);

    if(daemon)
      fatal(server_redirect, server);

    request * req;
    response * resp = 0;
    fatal(server_receive, server, &req);
    fatal(server_dispatch, server, req, mb, &resp);
    fatal(xchdir, "/");

    // task complete
    fatal(server_respond, server, mb, resp);

    fatal(usage_report);

    // cleanup tmp dir, including specifically the last fab pid we are tracking
    fatal(tmp_cleanup, &fab_pids[sizeof(fab_pids) / sizeof(fab_pids[0]) - 1], 1);

    // cycle fab pids
    memmove(&fab_pids[1], &fab_pids[0], sizeof(*fab_pids) * ((sizeof(fab_pids) / sizeof(fab_pids[0])) - 1));
    fab_pids[0] = server->pid;
  }

  // touch stamp file
  snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/stamp", g_params.pid);
  fatal(ixclose, &fd);
  fatal(uxopen_mode, space, O_CREAT | O_RDWR, FABIPC_DATA, &fd);
  if(fd != -1)
    fatal(xfutimens, fd, 0);

  // cycle in my own pid
  memmove(&fab_pids[1], &fab_pids[0], sizeof(*fab_pids) * ((sizeof(fab_pids) / sizeof(fab_pids[0])) - 1));
  fab_pids[0] = g_params.pid;

  // cleanup tmp dir, including specifically all of the fab pids we are tracking and my own pid
  fatal(tmp_cleanup, fab_pids, sizeof(fab_pids) / sizeof(fab_pids[0]));

finally:
  fatal(log_finish, &token);

#if DEBUG || DEVEL
  xapi_infos("name", "fabd");
  xapi_infof("pid", "%ld", (long)getpid());
  xapi_infos("hash", hash);
#endif

  if(XAPI_UNWINDING)
  {
    tracesz = xapi_trace_full(space, sizeof(space));
    xlogw(L_ERROR, L_RED, space, tracesz);
  }

  // locals
//  map_free(rmap);
//  map_free(vmap);
//  fabd_handler_context_free(ctx);
  fatal(ixclose, &fd);
  fatal(server_dispose, &server);
  fatal(memblk_free, mb);

  // modules
//  gn_teardown();
//  fml_teardown();
//  ff_teardown();
//  selector_teardown();

  // libraries
  fatal(xlinux_unload);
  fatal(listwise_unload);
  fatal(valyria_unload);
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(fabcore_unload);
  fatal(lorien_unload);

conclude(&R);
  xapi_teardown();

  return !!R;
}
