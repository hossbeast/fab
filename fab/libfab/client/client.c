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

#include <sys/syscall.h>
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
#include <linux/futex.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "xlinux/KERNEL.errtab.h"
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
#include "common/hash.h"
#include "common/attrs.h"

#include "client.internal.h"
#include "errtab/FAB.errtab.h"
#include "logging.internal.h"
#include "ipc.h"
#include "lockfile.h"
#include "sigutil.h"

#include "macros.h"
#include "barriers.h"
#include "atomics.h"

#if DEVEL
const char * APIDATA g_fab_client_fabd_path;
const char * APIDATA g_fab_client_system_config_path;
const char * APIDATA g_fab_client_user_config_path;
const char * APIDATA g_fab_client_project_config_path;
const char * APIDATA g_fab_client_default_filesystem_invalidate;
const char * APIDATA g_fab_client_sweeper_period_nsec;
#endif

/* ugo+rwx world read/write/execute */
#define IPCDIR_MODE_DIR   0777

/* ugo+rw  world read/write */
#define IPCDIR_MODE_DATA  0666

//
// static
//

static xapi launch(fab_client * restrict client)
{
  enter;

  sigset_t sigs;
  sigset_t oset;
  char *argv[16];
  int i;

  // canonical project directory symlink
  fatal(uxsymlinkf, "%s", "%s/projdir", client->projdir, client->ipcdir);

  // cleanup the directory
  fatal(uxunlinkf, "%s/fabd/exit", client->ipcdir);
  fatal(uxunlinkf, "%s/fabd/core", client->ipcdir);
  fatal(uxunlinkf, "%s/fabd/stdout", client->ipcdir);
  fatal(uxunlinkf, "%s/fabd/stderr", client->ipcdir);

  // block all signals
  sigfillset(&sigs);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, &oset);

  fatal(xfork, &client->fabd_pid);
  if(client->fabd_pid == 0)
  {
    // prepare the command
    i = 0;
    argv[i++] = "fabd";
    argv[i++] = client->hash;

#if DEBUG || DEVEL
    narrator * N;
    fatal(log_start, L_IPC, &N);
    xsayf("execv(");
#if TRACE || DEVEL
#if TRACE
    argv[0] = "fabd.trace.xapi";
#else
    argv[0] = "fabd.devel.xapi";
#endif
    xsays(g_fab_client_fabd_path);

    if(g_fab_client_system_config_path)
    {
      argv[i++] = "--system-config-path";
      argv[i++] = (char*)g_fab_client_system_config_path;
    }
    if(g_fab_client_user_config_path)
    {
      argv[i++] = "--user-config-path";
      argv[i++] = (char*)g_fab_client_user_config_path;
    }
    if(g_fab_client_project_config_path)
    {
      argv[i++] = "--project-config-path";
      argv[i++] = (char*)g_fab_client_project_config_path;
    }
    if(g_fab_client_default_filesystem_invalidate)
    {
      argv[i++] = "--default-filesystem-invalidate";
      argv[i++] = (char*)g_fab_client_default_filesystem_invalidate;
    }
    if(g_fab_client_sweeper_period_nsec)
    {
      argv[i++] = "--sweeper-period-nsec";
      argv[i++] = (char*)g_fab_client_sweeper_period_nsec;
    }
#else
    xsays("fabd");
#endif
    int x;
    for(x = 0; x < i; x++)
    {
      xsays(",");
      xsays(argv[x]);
    }
    xsays(")");
    fatal(log_finish);
#endif

    argv[i] = (void*)0;

    // reopen standard file descriptors
    fatal(xclose, 0);
    fatal(xopens, 0, O_RDONLY, "/dev/null");

    fatal(xclose, 1);
    fatal(xopen_modef, 0, O_WRONLY | O_CREAT | O_EXCL, IPCDIR_MODE_DATA, "%s/fabd/stdout", client->ipcdir);

    fatal(xclose, 2);
    fatal(xopen_modef, 0, O_WRONLY | O_CREAT | O_EXCL, IPCDIR_MODE_DATA, "%s/fabd/stderr", client->ipcdir);

    // core file goes in cwd
    fatal(xchdirf, "%s/fabd", client->ipcdir);

    // create new session/process group
    fatal(xsetsid);

    // update the lockfile
    fatal(fabipc_lockfile_update, "%s/fabd/lock", client->ipcdir);

#if DEVEL
    fatal(xexecv, g_fab_client_fabd_path, argv);
#else
    fatal(xexecvp, "fabd", argv);
#endif

    exit(1);
  }

  fatal(xpthread_sigmask, SIG_SETMASK, &oset, 0);

  finally : coda;
}

//
// public
//

xapi API fab_client_create(fab_client ** restrict clientp, const char * restrict projdir, const char * restrict ipcdir_base)
{
  enter;

  char space[512];
  size_t len;
  uint64_t u64;
  fab_client *client;

  fatal(xmalloc, &client, sizeof(*client));

  // canonicalized project path
  fatal(xrealpaths, &client->projdir, 0, projdir);

  // project path hash
  u64 = hash64(0, client->projdir, strlen(client->projdir));
  snprintf(client->hash, sizeof(client->hash), "%016"PRIx64, u64);

  // ipc dir for the project
  len = snprintf(space, sizeof(space), "%s/%s", ipcdir_base, client->hash);
  fatal(xmalloc, &client->ipcdir, len + 1);
  memcpy(client->ipcdir, space, len);

  client->msgid = 2;
  client->lockfd = -1;
  *clientp = client;

  finally : coda;
}

xapi API fab_client_xfree(fab_client * restrict client)
{
  enter;

  if(client)
  {
    wfree(client->ipcdir);
    wfree(client->projdir);
    fatal(xshmdt, client->shm);
    fatal(xclose, client->lockfd);
  }
  wfree(client);

  finally : coda;
}

xapi API fab_client_ixfree(fab_client ** restrict client)
{
  enter;

  fatal(fab_client_xfree, *client);
  *client = 0;

  finally : coda;
}

xapi API fab_client_prepare(fab_client * restrict client)
{
  enter;

  fatal(mkdirpf, IPCDIR_MODE_DIR, "%s/fabd", client->ipcdir);
  fatal(fabipc_lockfile_obtain, &client->fabd_pid, &client->lockfd, "%s/fabd/lock", client->ipcdir);

  finally : coda;
}

xapi API fab_client_kill(fab_client * restrict client)
{
  enter;

  fatal(uxkill, 0, client->fabd_pid, SIGTERM);

  /* force re-loading the fabd pid in solicit */
  fatal(ixclose, &client->lockfd);

  finally : coda;
}

xapi API fab_client_solicit(fab_client * restrict client)
{
  enter;

  int r;
  union sigval value;
  pid_t tid;

  if(client->lockfd == -1) {
    fatal(fabipc_lockfile_obtain, &client->fabd_pid, &client->lockfd, "%s/fabd/lock", client->ipcdir);
  }

  // lock obtained, server not running - launch the server
  if(client->fabd_pid == 0) {
    fatal(launch, client);
  }

  // send tid via signal to initialize the channel
  tid = gettid();
  value.sival_ptr = (void*)(intptr_t)tid;
  fatal(sigutil_uxrt_sigqueueinfo, &r, client->fabd_pid, SIGRTMIN, value);
  if(r) {
    fail(FAB_NODAEMON); // no such process
  }

  finally : coda;
}

xapi API fab_client_attach(fab_client * restrict client, int channel_shmid)
{
  enter;

  void * shmaddr = 0;

  /* channel shmid is the value of the signal */

  /* attach the channel (owned by the server) */
  fatal(xshmat, channel_shmid, 0, 0, &shmaddr);

  client->shm = shmaddr;
  shmaddr = 0;

  client->shm->client_pulse++;

  /* at this point the lockfile, if any, is now held by fabd and no longer needed */
  fatal(ixclose, &client->lockfd);

finally:
  fatal(xshmdt, shmaddr);
coda;
}

void API fab_client_disconnect(fab_client * restrict client)
{
  if(client->shm)
  {
    client->shm->client_exit = 1;
    syscall(SYS_tgkill, client->shm->server_pid, client->shm->server_tid, SIGUSR1);
  }
}

fabipc_message * API fab_client_produce(fab_client * restrict client)
{
  return fabipc_produce(
      client->shm->client_ring.pages
    , &client->shm->client_ring.head
    , &client->shm->client_ring.tail
    , &client->shm->client_ring.producers
    , &client->shm->client_ring.waiters
  );
}

void API fab_client_post(fab_client * restrict client, fabipc_message * restrict msg)
{
#if DEBUG || DEVEL
  RUNTIME_ASSERT(msg->type);
  if(msg->type == FABIPC_MSG_EVENTS) {
    RUNTIME_ASSERT(msg->evtype);
  }
#endif

  fabipc_post(
      msg
    , &client->shm->client_ring.producers
    , &client->shm->client_ring.waiters
  );
}

fabipc_message * API fab_client_acquire(fab_client * restrict client)
{
  fabipc_message *msg;
  msg = fabipc_acquire(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , &client->shm->server_ring.tail
  );

  return msg;
}

void API fab_client_consume(fab_client * restrict client, fabipc_message * restrict msg)
{
  fabipc_consume(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
    , &client->shm->server_ring.waiters
  );
}

void API fab_client_release(fab_client * restrict client, fabipc_message * restrict msg)
{
  fabipc_release(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
  );
}
