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
#include "fab/ipc.h"
#include "common/hash.h"
#include "common/attrs.h"

#include "client.internal.h"
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

static void launch(fab_client * restrict client)
{
  sigset_t sigs;
  sigset_t oset;
  char *argv[16];
  int i;

  // canonical project directory symlink
  uxsymlinkf("%s", "%s/projdir", client->projdir, client->ipcdir);

  // cleanup the directory
  uxunlinkf("%s/fabd/exit", client->ipcdir);
  uxunlinkf("%s/fabd/core", client->ipcdir);
  uxunlinkf("%s/fabd/stdout", client->ipcdir);
  uxunlinkf("%s/fabd/stderr", client->ipcdir);

  // block all signals
  sigfillset(&sigs);
  xpthread_sigmask(SIG_SETMASK, &sigs, &oset);

  client->fabd_pid = xfork();
  if(client->fabd_pid == 0)
  {
    // prepare the command
    i = 0;
    argv[i++] = "fabd";
    argv[i++] = client->hash;

#if DEBUG || DEVEL
    printf("execv(");
#if TRACE || DEVEL
#if TRACE
    argv[0] = "fabd.trace";
#else
    argv[0] = "fabd.devel";
#endif
    printf("%s", g_fab_client_fabd_path);

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
#endif
    int x;
    for(x = 0; x < i; x++)
    {
      printf(",");
      printf("%s", argv[x]);
    }
    printf(")");
#endif

    argv[i] = (void*)0;

    // reopen standard file descriptors
    xclose(0);
    xopens(O_RDONLY, "/dev/null");

    xclose(1);
    xopen_modef(O_WRONLY | O_CREAT | O_EXCL, IPCDIR_MODE_DATA, "%s/fabd/stdout", client->ipcdir);

    xclose(2);
    xopen_modef(O_WRONLY | O_CREAT | O_EXCL, IPCDIR_MODE_DATA, "%s/fabd/stderr", client->ipcdir);

    // core file goes in cwd
    xchdirf("%s/fabd", client->ipcdir);

    // create new session/process group
    xsetsid();

    // update the lockfile
    fabipc_lockfile_update("%s/fabd/lock", client->ipcdir);

#if DEVEL
    xexecv(g_fab_client_fabd_path, argv);
#else
    xexecvp("fabd", argv);
#endif

    exit(1);
  }

  xpthread_sigmask(SIG_SETMASK, &oset, 0);
}

//
// public
//

void API fab_client_create(fab_client ** restrict clientp, const char * restrict projdir, const char * restrict ipcdir_base)
{
  char space[512];
  size_t len;
  uint64_t u64;
  fab_client *client;

  xmalloc(&client, sizeof(*client));

  // canonicalized project path
  client->projdir = xrealpaths(0, projdir);

  // project path hash
  u64 = hash64(0, client->projdir, strlen(client->projdir));
  snprintf(client->hash, sizeof(client->hash), "%016"PRIx64, u64);

  // ipc dir for the project
  len = snprintf(space, sizeof(space), "%s/%s", ipcdir_base, client->hash);
  xmalloc(&client->ipcdir, len + 1);
  memcpy(client->ipcdir, space, len);

  client->msgid = 2;
  client->lockfd = -1;
  *clientp = client;
}

void API fab_client_xfree(fab_client * restrict client)
{
  if(client)
  {
    wfree(client->ipcdir);
    wfree(client->projdir);
    xshmdt(client->shm);
    xclose(client->lockfd);
  }
  wfree(client);
}

void API fab_client_ixfree(fab_client ** restrict client)
{
  fab_client_xfree(*client);
  *client = 0;
}

void API fab_client_prepare(fab_client * restrict client)
{
  mkdirpf(IPCDIR_MODE_DIR, "%s/fabd", client->ipcdir);
  fabipc_lockfile_obtain(&client->fabd_pid, &client->lockfd, "%s/fabd/lock", client->ipcdir);
}

void API fab_client_kill(fab_client * restrict client)
{
  uxkill(client->fabd_pid, SIGTERM);

  /* force re-loading the fabd pid in solicit */
  ixclose(&client->lockfd);
}

void API fab_client_solicit(fab_client * restrict client)
{
  int r;
  union sigval value;
  pid_t tid;

  if(client->lockfd == -1) {
    fabipc_lockfile_obtain(&client->fabd_pid, &client->lockfd, "%s/fabd/lock", client->ipcdir);
  }

  // lock obtained, server not running - launch the server
  if(client->fabd_pid == 0) {
    launch(client);
  }

  // send tid via signal to initialize the channel
  tid = gettid();
  value.sival_ptr = (void*)(intptr_t)tid;
  r = sigutil_uxrt_sigqueueinfo(client->fabd_pid, SIGRTMIN, value);
  if(r) {
    RUNTIME_ABORT(); // no such process
  }
}

void API fab_client_attach(fab_client * restrict client, int channel_shmid)
{
  void * shmaddr = 0;

  /* channel shmid is the value of the signal */

  /* attach the channel (owned by the server) */
  shmaddr = xshmat(channel_shmid, &shmaddr, 0);

  client->shm = shmaddr;

  client->shm->client_pulse++;

  /* at this point the lockfile, if any, is now held by fabd and no longer needed */
  ixclose(&client->lockfd);
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
    , FABIPC_CLIENT_RINGSIZE - 1
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

  fabipc_post(msg, &client->shm->client_ring.waiters);
}

fabipc_message * API fab_client_acquire(fab_client * restrict client)
{
  fabipc_message *msg;
  msg = fabipc_acquire(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , &client->shm->server_ring.tail
    , FABIPC_SERVER_RINGSIZE - 1
  );

  return msg;
}

void API fab_client_consume(fab_client * restrict client, fabipc_message * restrict msg)
{
  fabipc_consume(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
    , FABIPC_SERVER_RINGSIZE - 1
  );
}

void API fab_client_release(fab_client * restrict client, fabipc_message * restrict msg)
{
  fabipc_release(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
    , FABIPC_SERVER_RINGSIZE - 1
  );
}
