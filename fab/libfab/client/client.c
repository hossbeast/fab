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
#include "logging.internal.h"
#include "ipc.internal.h"

#include "sigutil.h"
#include "macros.h"
#include "hash.h"

struct fab_client
{
  char *    projdir;    // canonicalized
  char *    ipcdir;
  char      hash[16 + 1];
  char *    fabw_path;
  pid_t     pgid;       // fabd process group id
};

//
// static
//

static xapi pgid_load(fab_client * restrict client)
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

static xapi validate_result(fab_client * restrict client, int expsig, siginfo_t * actual)
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
      xapi_info_pushf("exit", "%d", WEXITSTATUS(exit));
      fail(FAB_NODAEMON);
    }
    else if(WIFSIGNALED(exit))
    {
      xapi_info_pushf("signal", "%d", WTERMSIG(exit));
      xapi_info_pushf("core", "%s", WCOREDUMP(exit) ? "yes" : "no");
      fail(FAB_NODAEMON);
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

API xapi fab_client_create(fab_client ** restrict client, const char * restrict projdir, const char * restrict ipcdir_base, const char * restrict fabw_path)
{
  enter;

  char space[512];
  size_t len;
  uint64_t u64;

  fatal(xmalloc, client, sizeof(**client));

  // canonicalized project path
  fatal(xrealpaths, &(*client)->projdir, 0, projdir);

  // project path hash
  u64 = hash64(0, (*client)->projdir, strlen((*client)->projdir));
  snprintf((*client)->hash, sizeof((*client)->hash), "%016"PRIx64, u64);

  // ipc dir for the project
  len = snprintf(space, sizeof(space), "%s/%s", ipcdir_base, (*client)->hash);
  fatal(xmalloc, &(*client)->ipcdir, len + 1);
  memcpy((*client)->ipcdir, space, len);

  if(fabw_path)
    fatal(ixstrdup, &(*client)->fabw_path, fabw_path);

  finally : coda;
}

API xapi fab_client_xfree(fab_client * restrict client)
{
  enter;

  if(client)
  {
    wfree(client->ipcdir);
    wfree(client->fabw_path);
    wfree(client->projdir);
    wfree(client);
  }

  finally : coda;
}

API xapi fab_client_ixfree(fab_client ** restrict client)
{
  enter;

  fatal(fab_client_xfree, *client);
  *client = 0;

  finally : coda;
}

API xapi fab_client_prepare(fab_client * restrict client)
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

API xapi fab_client_terminatep(fab_client * restrict client)
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

API xapi fab_client_launchp(fab_client * restrict client)
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

  // block signals
  sigfillset(&sigs);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, &oset);

  fatal(xfork, &client->pgid);
  if(client->pgid == 0)
  {
    // core file goes in cwd
    fatal(xchdirf, "%s/fabd", client->ipcdir);

    // create new session/process group
    fatal(xsetsid);

    // update the lockfile
    fatal(ipc_lock_update, "%s/fabd/lock", client->ipcdir);

    // daemonize
    fatal(xfork, &client->pgid);
    if(client->pgid != 0)
      exit(0);

    fatal(xmalloc, &argv, sizeof(*argv) * (g_logc + g_ulogc + 3));
    i = 0;
    argv[i++] = "fabw";
    argv[i++] = client->hash;

    for(x = 0; x < g_logc; x++)
      argv[i++] = g_logv[x];

    for(x = 0; x < g_ulogc; x++)
      argv[i++] = g_ulogv[x];

#if DEVEL
    argv[0] = "fabw.devel.xapi";
    narrator * N;
    fatal(log_start, L_IPC, &N);
    xsayf("execv(");
    xsays(client->fabw_path ?: "fabw");
    for(x = 0; x < i; x++)
    {
      xsays(",");
      xsays(argv[x]);
    }
    xsays(")");
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
  fatal(validate_result, client, FABIPC_SIGACK, &info);

finally:
  wfree(argv);
coda;
}

API xapi fab_client_prepare_request_shm(fab_client * restrict client, void ** restrict request_shm)
{
  enter;

  int fd = -1;
  void * shmaddr = 0;
  int shmid = -1;
  size_t REQUEST_SIZE = 0xfff;
  int request_shmid;

  // create shm for the request
  fatal(xshmget, ftok(client->ipcdir, FABIPC_TOKREQ), REQUEST_SIZE, IPC_CREAT | IPC_EXCL | FABIPC_MODE_DATA, &shmid);
  fatal(xshmat, shmid, 0, 0, &shmaddr);
  fatal(xshmctl, shmid, IPC_RMID, 0);
  request_shmid = shmid;
  shmid = -1;

  // save the shmid
  fatal(xopen_modef, &fd, O_CREAT | O_WRONLY, FABIPC_MODE_DATA, "%s/client/request_shmid", client->ipcdir);
  fatal(axwrite, fd, &request_shmid, sizeof(request_shmid));

  *request_shm = shmaddr;
  shmaddr = 0;

finally:
  fatal(ixclose, &fd);
  fatal(xshmdt, shmaddr);
  if(shmid != -1)
    fatal(xshmctl, shmid, IPC_RMID, 0);
coda;
}

API xapi fab_client_make_request(fab_client * restrict client, void * request_shm, void ** restrict response_shm)
{
  enter;

  int fd = -1;
  void * shmaddr = 0;
  int shmid;

  narrator * N;
  fab_message * msg = request_shm;
  fatal(log_start, L_PROTOCOL, &N);
  xsayf("request(%u)\n%s", msg->len, msg->text);
  fatal(log_finish);

  // awaken fabd and await response
  sigset_t sigs;
  sigfillset(&sigs);
  siginfo_t info;
  fatal(sigutil_exchange, FABIPC_SIGSYN, -client->pgid, &sigs, &info);
  fatal(validate_result, client, FABIPC_SIGACK, &info);

  // release the request
  fatal(ixshmdt, &request_shm);

  // attach response shms
  fatal(xopenf, &fd, O_RDONLY, "%s/fabd/response_shmid", client->ipcdir);
  fatal(axread, fd, &shmid, sizeof(shmid));
  fatal(xshmat, shmid, 0, 0, &shmaddr);

#if DEBUG || DEVEL
  msg = shmaddr;
  fatal(log_start, L_PROTOCOL, &N);
  xsayf("response(%u)\n%s", msg->len, msg->text);
  fatal(log_finish);
#endif

  *response_shm = shmaddr;
  shmaddr = 0;

finally:
  fatal(xshmdt, request_shm);
  fatal(xshmdt, shmaddr);
  fatal(ixclose, &fd);
coda;
}

API xapi fab_client_release_response(fab_client * restrict client, void * restrict response_shm)
{
  enter;

  fatal(xshmdt, response_shm);
  fatal(sigutil_kill, -client->pgid, FABIPC_SIGACK);

  finally : coda;
}

API char * fab_client_gethash(fab_client * restrict client)
{
  return client->hash;
}

API xapi fab_client_unlock(fab_client * restrict client)
{
  enter;

  fatal(ipc_lock_release, "%s/client/lock", client->ipcdir);

  finally : coda;
}
