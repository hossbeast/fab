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

#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstat.h"
#include "xlinux/xstring.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"

#include "narrator.h"
#include "narrator/units.h"
#include "lorien/mkdirp.h"
#include "lorien/symlinkp.h"
#include "logger.h"
#include "fab/ipc.h"

#include "internal.h"
#include "client.internal.h"
#include "errtab/FAB.errtab.h"
#include "sigbank.internal.h"
#include "request.internal.h"
#include "response.internal.h"
#include "identity.internal.h"
#include "logging.internal.h"
#include "ipc.internal.h"

#include "memblk.h"
#include "memblk.def.h"
#include "macros.h"
#include "cksum.h"

#define restrict __restrict

struct fab_client
{
  char *    projdir;    // canonicalized
  char *    ipcdir;
  char      hash[8];
  char *    fabw_path;
  pid_t     pgid;       // fabd process group id
};

//
// static
//

static xapi pgid_load(fab_client * const restrict client)
{
  enter;

  char space[512];
  int fd = -1;

  client->pgid = 0;

  snprintf(space, sizeof(space), "%s/fabd/pgid", client->ipcdir);
  fatal(uxopens, &fd, O_RDONLY, space);

  if(fd != -1)
  {
    // read and validate the pid
    fatal(axread, fd, &client->pgid, sizeof(client->pgid));
    if(client->pgid <= 0)
    {
      xapi_fail_intent();
      xapi_info_adds("expected pgid", "> 0");
      xapi_info_addf("actual pgid", "%ld", (long)client->pgid);
      fail(FAB_BADIPC);
    }
  }

finally:
  fatal(ixclose, &fd);
coda;
}

static xapi verify_result(fab_client * const restrict client, int expsig, int actsig)
{
  enter;

  char space[512];
  int exit;
  int fd = -1;

  if(actsig == FABIPC_SIGEND)
  {
    // check child exit file
    snprintf(space, sizeof(space), "%s/fabd/exit", client->ipcdir);
    fatal(ixclose, &fd);
    fatal(xopens, &fd, O_RDONLY, space);
    fatal(axread, fd, MM(exit));

    xapi_fail_intent();

    if(WIFEXITED(exit))
    {
      xapi_info_addf("status", "%d", WEXITSTATUS(exit));
      fail(FAB_FABDEXIT);
    }
    else if(WIFSIGNALED(exit))
    {
      xapi_info_addf("signal", "%d", WTERMSIG(exit));
      fail(FAB_FABDFAIL);
    }
    fail(FAB_BADIPC);
  }

  fatal(sigbank_assert, expsig, actsig, 0, 0);

finally:
  fatal(ixclose, &fd);
coda;
}

//
// public
//

API xapi fab_client_create(fab_client ** const restrict client, const char * const restrict projdir, const char * const restrict ipcdir_base, const char * const restrict fabw_path)
{
  enter;

  char space[512];

  fatal(xmalloc, client, sizeof(**client));

  // canonicalized project path
  fatal(xrealpath, projdir, space);
  fatal(ixstrdup, &(*client)->projdir, space);

  // project path hash
  uint32_t u32 = cksum((*client)->projdir, strlen((*client)->projdir));
  snprintf((*client)->hash, sizeof((*client)->hash), "%x", u32);

  // ipc dir for the project
  snprintf(space, sizeof(space), "%s/%s", ipcdir_base, (*client)->hash);
  fatal(ixstrdup, &(*client)->ipcdir, space);

  if(fabw_path)
    fatal(ixstrdup, &(*client)->fabw_path, fabw_path);

  finally : coda;
}

API xapi fab_client_dispose(fab_client ** const restrict client)
{
  enter;

  if(*client)
  {
    wfree((*client)->ipcdir);
    wfree((*client)->fabw_path);
    wfree(*client);
  }

  *client = 0;

  finally : coda;
}

API xapi fab_client_prepare(fab_client * const restrict client)
{
  enter;

  char space[512];
  pid_t pid;
  int fd = -1;

  // client directory
  fatal(mkdirpf, FABIPC_MODE_DIR, "%s/client", client->ipcdir);

  // obtain exclusive lock
  while(1)
  {
    pid_t pgid;
    fatal(ipc_lock_obtain, &pgid, "%s/client/lock", client->ipcdir);

    // lock obtained
    if(pgid == 0)
      break;

    if(pgid == -1)
      continue;

    int r;
    fatal(uxkill, -pgid, 0, &r);

    // already running
    if(r == 0)
      failf(FAB_FABLOCK, "pgid", "%ld", (long)pgid);

    // not running ; forcibly release the lock, try again
    fatal(ipc_lock_release, "%s/client/lock", client->ipcdir);
  }

  // canonical projdir symlink
  fatal(uxsymlinkf, "%s", "%s/projdir", client->projdir, client->ipcdir);

  // canonical cwd symlink
  fatal(symlinkpf, "%s", "%s/client/cwd", getcwd(space, sizeof(space)), client->ipcdir);

//  snprintf(space, sizeof(space), "%s/client/cwd", client->ipcdir);
//  fatal(uxunlink, space);
//  fatal(xsymlink, g_params.cwd, space);

  pid = getpid();

  // client stdout symlink
  ssize_t ss = 0;
  fatal(xreadlinkf, "/proc/%ld/fd/1", space, sizeof(space), &ss, (long)pid);
  if(ss >= sizeof(space))
    ss = sizeof(space) - 1;
  if(ss >= 0)
    space[ss] = 0;
  fatal(symlinkpf, "%s", "%s/client/out", space, client->ipcdir);

  // client stderr symlink
  ss = 0;
  fatal(xreadlinkf, "/proc/%ld/fd/2", space, sizeof(space), &ss, (long)pid);
  if(ss >= sizeof(space))
    ss = sizeof(space) - 1;
  if(ss >= 0)
    space[ss] = 0;
  fatal(symlinkpf, "%s", "%s/client/err", space, client->ipcdir);

#if 0
  snprintf(space2, sizeof(space2), "/proc/%ld/fd/1", (long)pid);
  ssize_t ss = 0;
  fatal(xreadlink, space2, space2, sizeof(space2), &ss);
  if(ss >= sizeof(space2))
    ss = sizeof(space2) - 1;
  if(ss >= 0)
    space2[ss] = 0;
  snprintf(space, sizeof(space), "%s/client/out", client->ipcdir);
  fatal(uxunlink, space);
  fatal(xsymlink, space2, space);
#endif

#if 0
  // client stderr symlink
  snprintf(space2, sizeof(space2), "/proc/%ld/fd/2", (long)pid);
  ss = 0;
  fatal(xreadlink, space2, space2, sizeof(space2), &ss);
  if(ss >= sizeof(space2))
    ss = sizeof(space2) - 1;
  if(ss >= 0)
    space2[ss] = 0;
  snprintf(space, sizeof(space), "%s/client/err", client->ipcdir);
  fatal(uxunlink, space);
  fatal(xsymlink, space2, space);
#endif

  // client pid file
  snprintf(space, sizeof(space), "%s/client/pid", client->ipcdir);
  fatal(xopen_modes, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, space);
  fatal(axwrite, fd, &pid, sizeof(pid));
  fatal(ixclose, &fd);

  // fabd directories
  fatal(mkdirpf, FABIPC_MODE_DIR, "%s/fabd", client->ipcdir);

finally:
  fatal(ixclose, &fd);
coda;
}

API xapi fab_client_terminatep(fab_client * const restrict client)
{
  enter;

  fatal(pgid_load, client);
  if(client->pgid)
  {
    // existence assertion
    fatal(xkill, -client->pgid, 0);
  }

  finally : coda;
}

API xapi fab_client_launchp(fab_client * const restrict client)
{
  enter;

  int token = 0;
  int r;

  // attempt to get the fabd lock
  while(1)
  {
    fatal(ipc_lock_obtain, &client->pgid, "%s/fabd/lock", client->ipcdir);

    // lock obtained
    if(client->pgid == 0)
      break;

    if(client->pgid == -1)
      continue;

    // check whether the lock holder is still running
    fatal(uxkill, -client->pgid, 0, &r);

    // already running
    if(r == 0)
      break;

    // not running ; forcibly release the lock, try again
    fatal(ipc_lock_release, "%s/fabd/lock", client->ipcdir);
  }

  // daemon is already running
  if(client->pgid)
    goto XAPI_FINALIZE;

  fatal(identity_assume_fabsys);
  fatal(xfork, &client->pgid);
  if(client->pgid == 0)
  {
    // cleanup the directory
    fatal(uxunlinkf, "%s/fabd/exit", client->ipcdir);
    fatal(uxunlinkf, "%s/fabd/core", client->ipcdir);
    fatal(uxunlinkf, "%s/fabd/trace", client->ipcdir);

    // create new session/process group
    fatal(xsetsid);

    // update the lockfile
    fatal(ipc_lock_update, "%s/fabd/lock", client->ipcdir);

    // daemonize
    fatal(xfork, &client->pgid);
    if(client->pgid != 0)
      exit(0);

    char * argv[] = {
#if DEVEL
        "fabw.devel"
#else
        "fabw"
#endif
      , "+IPC"
      , client->hash, (void*)0
    };

#if DEVEL
    fatal(log_start, L_IPC, &token);
    logf(0, "execv(");
    logs(0, client->fabw_path ?: "fabw");
    int x;
    for(x = 0; x < sentinel(argv); x++)
    {
      logs(0, ",");
      logs(0, argv[x]);
    }
    logs(0, ")");
    fatal(log_finish, &token);
#endif

    if(client->fabw_path)
      fatal(execv, client->fabw_path, argv);
    else
      fatal(execvp, "fabw", argv);
  }

  // await ready signal from the child
#if DEVEL
//logf(L_IPC, "client waiting to receive ACK(%d)", FABIPC_SIGACK);
#endif
  int actsig;
  fatal(sigbank_receive, &actsig, 0);
#if DEVEL
//logf(L_IPC, "client received %d", actsig);
#endif
  fatal(identity_assume_user);

  fatal(verify_result, client, FABIPC_SIGACK, actsig);

finally:
  fatal(log_finish, &token);
coda;
}

API xapi fab_client_request_make(fab_client * const restrict client, memblk * const restrict mb, fab_request * const restrict request)
{
  enter;

  fab_response * response = 0;
  int token = 0;
  int shmid_tmp;
  int fd = -1;
  int req_shmid = -1;
  int res_shmid = -1;
  void * shmaddr = 0;

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logs(0, "request ");
  fatal(fab_request_say, request, log_narrator(&token));
  fatal(log_finish, &token);
#endif

  // create shm for the request
  fab_request_freeze(request, mb);
  fatal(xshmget, ftok(client->ipcdir, FABIPC_TOKREQ), memblk_size(mb), IPC_CREAT | IPC_EXCL | FABIPC_MODE_DATA, &req_shmid);
  fatal(xshmat, req_shmid, 0, 0, &shmaddr);
  fatal(xshmctl, req_shmid, IPC_RMID, 0);
  shmid_tmp = req_shmid;
  req_shmid = -1;

  // save the shmid
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/client/request_shmid", client->ipcdir);
  fatal(axwrite, fd, &shmid_tmp, sizeof(shmid_tmp));
  fatal(ixclose, &fd);

  // write the request to shm
  memblk_copyto(mb, shmaddr, memblk_size(mb));

  // awaken fabd and await response
  int actsig;
  fatal(sigbank_exchange, FABIPC_SIGSYN, -client->pgid, &actsig, 0);
  fatal(verify_result, client, FABIPC_SIGACK, actsig);

  // destroy the request
  fatal(ixshmdt, &shmaddr);

  // attach to shm for the response
  fatal(xopenf, &fd, O_RDONLY, "%s/fabd/response_shmid", client->ipcdir);
  fatal(axread, fd, &res_shmid, sizeof(res_shmid));
  fatal(ixclose, &fd);
  fatal(xshmat, res_shmid, 0, 0, &shmaddr);

  // consume the response
  response = shmaddr;
  fab_response_thaw(response, shmaddr);

#if DEBUG || DEVEL
  fatal(log_start, L_IPC, &token);
  logs(0, "response ");
  fatal(fab_response_say, response, log_narrator(&token));
  fatal(log_finish, &token);
#endif

  // unblock fabd
  fatal(xkill, -client->pgid, FABIPC_SIGACK);

finally:
  fatal(log_finish, &token);
  fatal(ixshmdt, &shmaddr);
  if(req_shmid != -1)
    fatal(xshmctl, req_shmid, IPC_RMID, 0);
  fatal(ixclose, &fd);
coda;
}

API char * fab_client_gethash(fab_client * const restrict client)
{
  return client->hash;
}
