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

#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstat.h"
#include "xlinux/xstring.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"
#include "xlinux/xpthread.h"

#include "narrator.h"
#include "narrator/units.h"
#include "lorien/mkdirp.h"
#include "lorien/symlinkp.h"
#include "logger.h"
#include "logger/arguments.h"
#include "fab/ipc.h"

#include "internal.h"
#include "client.internal.h"
#include "errtab/FAB.errtab.h"
#include "request.internal.h"
#include "response.internal.h"
#include "identity.internal.h"
#include "logging.internal.h"
#include "ipc.internal.h"

#include "sigutil.h"
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
      xapi_info_pushs("expected pgid", "> 0");
      xapi_info_pushf("actual pgid", "%ld", (long)client->pgid);
      fail(FAB_BADIPC);
    }
  }

finally:
  fatal(ixclose, &fd);
coda;
}

static xapi validate_result(fab_client * const restrict client, int expsig, siginfo_t * actual)
{
  enter;

  int exit;
  int fd = -1;

  // SIGEND is sent by fabw
  if(actual->si_signo == FABIPC_SIGEND)
  {
    fatal(ixclose, &fd);
    fatal(xopenf, &fd, O_RDONLY, "%s/fabd/exit", client->ipcdir);
    fatal(axread, fd, MM(exit));

    if(WIFEXITED(exit))
    {
      // exited means orderly shutdown
      xapi_info_pushf("status", "%d", WEXITSTATUS(exit));
      fail(FAB_DAEMONEXITED);
    }
    else if(WIFSIGNALED(exit))
    {
      // failure means abnormal termination
      xapi_info_pushf("signal", "%d", WTERMSIG(exit));
      fail(FAB_DAEMONFAILED);
    }
    fail(FAB_BADIPC);
  }

  fatal(sigutil_assert, expsig, 0, actual);

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
  fatal(xrealpaths, &(*client)->projdir, 0, projdir);

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
  pid_t pgid;
  int fd = -1;

  // client directory
  fatal(mkdirpf, FABIPC_MODE_DIR, "%s/client", client->ipcdir);

  // obtain the client lock
  fatal(ipc_lock_obtain, &pgid, 0, "%s/client/lock", client->ipcdir);

  // client already running
  if(pgid)
    failf(FAB_CLIENTEXCL, "pgid", "%ld", (long)pgid);

  // canonical projdir symlink
  fatal(uxsymlinkf, "%s", "%s/projdir", client->projdir, client->ipcdir);

  // canonical cwd symlink
  fatal(symlinkpf, "%s", "%s/client/cwd", getcwd(space, sizeof(space)), client->ipcdir);

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

  sigset_t sigs;
  sigset_t oset;
  char ** argv = 0;
  int i;
  int x;

  // attempt to obtain the server lock
  fatal(ipc_lock_obtain, &client->pgid, 0, "%s/fabd/lock", client->ipcdir);

  // server daemon is already running
  if(client->pgid)
    goto XAPI_FINALIZE;

  // cleanup the directory
  fatal(uxunlinkf, "%s/fabd/exit", client->ipcdir);
  fatal(uxunlinkf, "%s/fabd/core", client->ipcdir);
  fatal(uxunlinkf, "%s/fabd/trace", client->ipcdir);

  // block signals
  sigfillset(&sigs);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, &oset);

  fatal(xfork, &client->pgid);
  if(client->pgid == 0)
  {
    // create new session/process group
    fatal(xsetsid);

    // update the lockfile
    fatal(ipc_lock_update, "%s/fabd/lock", client->ipcdir);

    // daemonize
    fatal(xfork, &client->pgid);
    if(client->pgid != 0)
      exit(0);

    // core file goes in cwd
    fatal(xchdirf, "%s/fabd", client->ipcdir);

    fatal(xmalloc, &argv, sizeof(*argv) * (g_logc + g_ulogc + 3));
    i = 0;
    argv[i++] = "fabw";
    argv[i++] = client->hash;

    for(x = 0; x < g_logc; x++)
      argv[i++] = g_logv[x];

    for(x = 0; x < g_ulogc; x++)
      argv[i++] = g_ulogv[x];

#if DEVEL
    argv[0] = "fabw.devel";
    narrator * N;
    fatal(log_start, L_IPC, &N);
    sayf("execv(");
    says(client->fabw_path ?: "fabw");
    for(x = 0; x < i; x++)
    {
      says(",");
      says(argv[x]);
    }
    says(")");
    fatal(log_finish);
#endif

    if(client->fabw_path)
      fatal(xexecv, client->fabw_path, argv);
    else
      fatal(xexecvp, "fabw", argv);
  }

  fatal(xpthread_sigmask, SIG_SETMASK, &oset, 0);

  // await ready signal from the child
  sigfillset(&sigs);
  siginfo_t info;
  while(1)
  {
    fatal(sigutil_wait, &sigs, &info);
    if(info.si_signo == SIGCHLD)
      continue;
    break;
  }
  fatal(identity_assume_real);
  fatal(validate_result, client, FABIPC_SIGACK, &info);

finally:
  wfree(argv);
coda;
}

API xapi fab_client_make_request(fab_client * const restrict client, memblk * const restrict mb, fab_request * const restrict request)
{
  enter;

  int shmid_tmp;
  int fd = -1;
  int req_shmid = -1;
  int res_shmid = -1;
  void * shmaddr = 0;

#if DEBUG || DEVEL
  narrator * N;

  fatal(log_start, L_PROTOCOL, &N);
  says("request ");
  fatal(fab_request_say, request, N);
  fatal(log_finish);
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
  sigset_t sigs;
  sigfillset(&sigs);
  siginfo_t info;
  fatal(sigutil_exchange, FABIPC_SIGSYN, -client->pgid, &sigs, &info);
  fatal(validate_result, client, FABIPC_SIGACK, &info);

  // destroy the request
  fatal(ixshmdt, &shmaddr);

  // attach to shm for the response
  fatal(xopenf, &fd, O_RDONLY, "%s/fabd/response_shmid", client->ipcdir);
  fatal(axread, fd, &res_shmid, sizeof(res_shmid));
  fatal(ixclose, &fd);
  fatal(xshmat, res_shmid, 0, 0, &shmaddr);

  // consume the response
  fab_response * response = shmaddr;
  fab_response_thaw(response, shmaddr);

#if DEBUG || DEVEL
  fatal(log_start, L_PROTOCOL, &N);
  says("response ");
  fatal(fab_response_say, response, N);
  fatal(log_finish);
#endif

  // unblock fabd
//printf("%10s (%d) : tx : %s (%d) -> %d\n", "client", gettid(), FABIPC_SIGNAME(FABIPC_SIGACK), FABIPC_SIGACK, -client->pgid);
  fatal(xkill, -client->pgid, FABIPC_SIGACK);

//  if(response->exit)
//    fail(FAB_UNSUCCESS);

finally:
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
