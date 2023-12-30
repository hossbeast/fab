/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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
#include "common/hash.h"
#include "common/attrs.h"

#include "client.h"
#include "ipc.h"
#include "lockfile.h"
#include "sigutil.h"

#include "macros.h"
#include "barriers.h"
#include "atomics.h"

#if CONFIG_DEVEL
const char * APIDATA g_bam_client_bamd_path;
const char * APIDATA g_bam_client_system_config_path;
const char * APIDATA g_bam_client_user_config_path;
const char * APIDATA g_bam_client_project_config_path;
const char * APIDATA g_bam_client_default_filesystem_invalidate;
const char * APIDATA g_bam_client_sweeper_period_nsec;
#endif

/* ugo+rwx world read/write/execute */
#define IPCDIR_MODE_DIR   0777

/* ugo+rw  world read/write */
#define IPCDIR_MODE_DATA  0666

//
// static
//

static void launch(bam_client * restrict client)
{
  sigset_t sigs;
  sigset_t oset;
  char *argv[16];
  int i;

  // canonical project directory symlink
  uxunlinkf("%s/projdir", client->ipcdir);
  xsymlinkf("%s", "%s/projdir", client->projdir, client->ipcdir);

  // cleanup the directory
  uxunlinkf("%s/bamd/exit", client->ipcdir);
  uxunlinkf("%s/bamd/core", client->ipcdir);
  uxunlinkf("%s/bamd/stdout", client->ipcdir);
  uxunlinkf("%s/bamd/stderr", client->ipcdir);

  // block all signals
  sigfillset(&sigs);
  xpthread_sigmask(SIG_SETMASK, &sigs, &oset);

  client->bamd_pid = xfork();
  if(client->bamd_pid == 0)
  {
    // prepare the command
    i = 0;
    argv[i++] = "bamd";
    argv[i++] = client->hash;

#if CONFIG_DEVEL
    if(g_bam_client_system_config_path)
    {
      argv[i++] = "--system-config-path";
      argv[i++] = (char*)g_bam_client_system_config_path;
    }
    if(g_bam_client_user_config_path)
    {
      argv[i++] = "--user-config-path";
      argv[i++] = (char*)g_bam_client_user_config_path;
    }
    if(g_bam_client_project_config_path)
    {
      argv[i++] = "--project-config-path";
      argv[i++] = (char*)g_bam_client_project_config_path;
    }
    if(g_bam_client_default_filesystem_invalidate)
    {
      argv[i++] = "--default-filesystem-invalidate";
      argv[i++] = (char*)g_bam_client_default_filesystem_invalidate;
    }
    if(g_bam_client_sweeper_period_nsec)
    {
      argv[i++] = "--sweeper-period-nsec";
      argv[i++] = (char*)g_bam_client_sweeper_period_nsec;
    }
#endif
    argv[i] = (void*)0;
#if CONFIG_DEVEL || CONFIG_DEBUG
#if CONFIG_DEVEL
    printf("execv(%s", g_bam_client_bamd_path);
#elif CONFIG_DEBUG
    printf("execvp(%s", "bamd");
#endif
#endif
#if CONFIG_DEBUG || CONFIG_DEVEL
    int x;
    for(x = 0; x < i; x++) {
      printf(",%s", argv[x]);
    }
    printf(")");
#endif

    // reopen standard file descriptors
    xclose(0);
    xopens(O_RDONLY, "/dev/null");

    xclose(1);
    xopen_modef(O_WRONLY | O_CREAT | O_EXCL, IPCDIR_MODE_DATA, "%s/bamd/stdout", client->ipcdir);

    xclose(2);
    xopen_modef(O_WRONLY | O_CREAT | O_EXCL, IPCDIR_MODE_DATA, "%s/bamd/stderr", client->ipcdir);

    // core file goes in cwd
    xchdirf("%s/bamd", client->ipcdir);

    // create new session/process group
    xsetsid();

    // update the lockfile
    bamipc_lockfile_update("%s/bamd/lock", client->ipcdir);

#if CONFIG_DEVEL
    xexecv(g_bam_client_bamd_path, argv);
#else
    xexecvp("bamd", argv);
#endif

    exit(1);
  }

  xpthread_sigmask(SIG_SETMASK, &oset, 0);
}

//
// public
//

void API bam_client_create(bam_client ** restrict clientp, const char * restrict projdir, const char * restrict ipcdir_base)
{
  char space[PATH_MAX];
  size_t len;
  uint64_t u64;
  bam_client *client;

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

void API bam_client_xfree(bam_client * restrict client)
{
  if(client)
  {
    free(client->ipcdir);
    free(client->projdir);
    xshmdt(client->shm);
    xclose(client->lockfd);
  }
  free(client);
}

void API bam_client_ixfree(bam_client ** restrict client)
{
  bam_client_xfree(*client);
  *client = 0;
}

void API bam_client_prepare(bam_client * restrict client)
{
  mkdirpf(IPCDIR_MODE_DIR, "%s/bamd", client->ipcdir);
  bamipc_lockfile_obtain(&client->bamd_pid, &client->lockfd, "%s/bamd/lock", client->ipcdir);
}

void API bam_client_kill(bam_client * restrict client)
{
  uxkill(client->bamd_pid, SIGTERM);

  /* force re-loading the bamd pid in solicit */
  ixclose(&client->lockfd);
}

void API bam_client_solicit(bam_client * restrict client)
{
  int r;
  union sigval value;
  pid_t tid;

  if(client->lockfd == -1) {
    bamipc_lockfile_obtain(&client->bamd_pid, &client->lockfd, "%s/bamd/lock", client->ipcdir);
  }

  // lock obtained, server not running - launch the server
  if(client->bamd_pid == 0) {
    launch(client);
  }

  // send tid via signal to initialize the channel
  tid = gettid();
  value.sival_ptr = (void*)(intptr_t)tid;
  r = sigutil_uxrt_sigqueueinfo(client->bamd_pid, SIGRTMIN, value);
  if(r) {
    RTA; // no such process
  }
}

void API bam_client_attach(bam_client * restrict client, int channel_shmid)
{
  void * shmaddr = 0;

  /* channel shmid is the value of the signal */

  /* attach the channel (owned by the server) */
  shmaddr = xshmat(channel_shmid, 0, 0);

  client->shm = shmaddr;
  client->shm->client_pulse++;

  /* at this point the lockfile, if any, is now held by bamd and no longer needed */
  ixclose(&client->lockfd);
}

void API bam_client_disconnect(bam_client * restrict client)
{
  if(client->shm)
  {
    client->shm->client_exit = 1;
    syscall(SYS_futex, &client->shm->client_ring.waiters, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  }
}

bamipc_message * API bam_client_produce(bam_client * restrict client)
{
  return bamipc_produce(
      client->shm->client_ring.pages
    , &client->shm->client_ring.head
    , &client->shm->client_ring.tail
    , &client->shm->client_ring.producers
    , &client->shm->client_ring.waiters
  );
}

void API bam_client_post(bam_client * restrict client, bamipc_message * restrict msg)
{
#if CONFIG_DEBUG || CONFIG_DEVEL
  RUNTIME_ASSERT(msg->type);
  if(msg->type == BAMIPC_MSG_EVENTS) {
    RUNTIME_ASSERT(msg->evtype);
  }
#endif

  bamipc_post(
      msg
    , &client->shm->client_ring.producers
    , &client->shm->client_ring.waiters
  );
}

bamipc_message * API bam_client_acquire(bam_client * restrict client)
{
  bamipc_message *msg;
  msg = bamipc_acquire(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , &client->shm->server_ring.tail
  );

  return msg;
}

void API bam_client_consume(bam_client * restrict client, bamipc_message * restrict msg)
{
  bamipc_consume(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
    , &client->shm->server_ring.waiters
  );
}

void API bam_client_release(bam_client * restrict client, bamipc_message * restrict msg)
{
  bamipc_release(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
  );
}
