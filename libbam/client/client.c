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
#include "common/parseint.h"
#include "common/attrs.h"

#include "client.h"
#include "ipc.h"
#include "lockfile.h"
#include "sigutil.h"

#include "macros.h"
#include "barriers.h"
#include "atomics.h"
#include "zcat.h"

#if CONFIG_DEVEL
const char * g_bam_client_bamd_path;
const char * g_bam_client_system_config_path;
const char * g_bam_client_user_config_path;
const char * g_bam_client_default_filesystem_invalidation;
const char * g_bam_client_sweeper_period_nsec;
#endif

/* ugo+rwx world read/write/execute */
#define IPCDIR_MODE_DIR   0777

/* ugo+rw  world read/write */
#define IPCDIR_MODE_DATA  0666

//
// static
//

int64_t lockfile_read(int fd);

//
// public
//

void bam_client_init(
    bam_client * restrict client
  , const char * restrict basedir
  , uint16_t basedir_len
  , const char * restrict ipcdir
  , uint16_t ipcdir_len
)
{
  char space[PATH_MAX];
  size_t len;

  memset(client, 0, sizeof(*client));

  // canonicalized project path
  client->basedir = xrealpathw(0, basedir, basedir_len);
  client->basedir_len = strlen(client->basedir);

  // project path hash
  client->hash = hash64(0, client->basedir, client->basedir_len);
  snprintf(client->hash_str, sizeof(client->hash_str), "%016"PRIx64, client->hash);

  /* base ipc dir */
  xmalloc(&client->ipcdir_base, ipcdir_len + 1);
  memcpy(client->ipcdir_base, ipcdir, ipcdir_len);
  client->ipcdir_base_len = ipcdir_len;

  // ipc dir for the project
  len = snprintf(space, sizeof(space), "%.*s/%s", (int)ipcdir_len, ipcdir, client->hash_str);
  xmalloc(&client->ipcdir, len + 1);
  memcpy(client->ipcdir, space, len);
  client->ipcdir_len = len;

  client->msgid = 2;
  client->lockfd = -1;
  client->bamd_pid = -1;
}

void bam_client_destroy(bam_client * restrict client)
{
  free(client->ipcdir_base);
  free(client->ipcdir);
  free(client->basedir);
  xshmdt(client->shm);
  xclose(client->lockfd);
}

void bam_client_lockfile_query(bam_client * restrict client)
{
  char path[PATH_MAX];
  char *buf;
  size_t sz;
  size_t z;

  if(client->lockfd != -1) {
    return;
  }

  buf = path;
  sz = sizeof(path);
  z = 0;

  z += zcatf(buf + z, sz - z, "%s/bamd", client->ipcdir);
  buf[z] = 0;
  mkdirpf(IPCDIR_MODE_DIR, "%s", path);

  z += zcats(buf + z, sz - z, "/lock");
  buf[z] = 0;
  bamipc_lockfile_acquire(&client->bamd_pid, &client->lockfd, path);//"%s/bamd/lock", client->ipcdir);
}

void bam_client_lockfile_validate(bam_client * restrict client)
{
  char path[PATH_MAX];
  char *buf;
  size_t sz;
  size_t z;

  buf = path;
  sz = sizeof(path);
  z = 0;

  z += zcatf(buf + z, sz - z, "%s/bamd/lock", client->ipcdir);
  buf[z] = 0;
  bamipc_lockfile_validate(&client->bamd_pid, &client->lockfd, path);
}

void bam_client_launch(bam_client * restrict client)
{
  sigset_t sigs;
  sigset_t oset;
  char *argv[16];
  int i;
  int x;

  /* lock must have been obtained previously */
  RUNTIME_ASSERT(client->bamd_pid == -1);
  RUNTIME_ASSERT(client->lockfd != -1);

  /* prepare the ipc dir */
  uxunlinkf("%s/basedir", client->ipcdir);
  xsymlinkf("%s", "%s/basedir", client->basedir, client->ipcdir);

  // cleanup the directory
  uxunlinkf("%s/bamd/exit", client->ipcdir);
  uxunlinkf("%s/bamd/core", client->ipcdir);
  uxunlinkf("%s/bamd/stdout", client->ipcdir);
  uxunlinkf("%s/bamd/stderr", client->ipcdir);

  // block all signals
  sigfillset(&sigs);
  xpthread_sigmask(SIG_SETMASK, &sigs, &oset);

  if((client->bamd_pid = xfork()) == 0)
  {
    // prepare the command
    i = 0;
    argv[i++] = "bamd";
    argv[i++] = client->hash_str;

    /* its a hack, but this is a simple way to give bamd a visible place to record the base path */
    argv[i++] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

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
    if(g_bam_client_default_filesystem_invalidation)
    {
      argv[i++] = "--default-filesystem-invalidation";
      argv[i++] = (char*)g_bam_client_default_filesystem_invalidation;
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

    /* place the lockfd in a known location */
    close(250);
    xdup2(client->lockfd, 250);

    /* close all other file descriptors */
    for(x = 3; x < 512; x++) {
      if(x == 250) {
        continue;
      }
      close(x);
    }

    // core file goes in cwd
    xchdirf("%s/bamd", client->ipcdir);

    // create new session/process group
    xsetsid();

    // update the lockfile
    bamipc_lockfile_update(250);

#if CONFIG_DEVEL
    xexecv(g_bam_client_bamd_path, argv);
#else
    xexecvp("bamd", argv);
#endif

    exit(1);
  }

  xpthread_sigmask(SIG_SETMASK, &oset, 0);
  ixclose(&client->lockfd);
}

int bam_client_solicit(bam_client * restrict client)
{
  int r;
  union sigval value;
  int64_t tid;

  /* must have been loaded previously */
  RUNTIME_ASSERT(client->bamd_pid != -1);

  // send tid via signal to initialize the channel
  tid = gettid();
  value.sival_ptr = (void*)(intptr_t)tid;
  r = sigutil_uxrt_sigqueueinfo(client->bamd_pid, SIGRTMIN, value);

  return r;
}

void bam_client_attach(bam_client * restrict client, int channel_shmid)
{
  void *shmaddr = 0;

  /* channel shmid is the value of the signal */

  /* attach the channel (owned by the server) */
  shmaddr = xshmat(channel_shmid, 0, 0);

  client->shm = shmaddr;
  client->shm->client_pulse++;

  /* at this point the lockfile, if any, is now held by bamd and no longer needed */
  ixclose(&client->lockfd);
}

void bam_client_disconnect(bam_client * restrict client)
{
  if(!client->shm) {
    return;
  }
  client->shm->client_exit = 1;
  syscall(SYS_futex, &client->shm->client_ring.waiters, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
}

bamipc_message * bam_client_produce(bam_client * restrict client)
{
  return bamipc_produce(
      client->shm->client_ring.pages
    , &client->shm->client_ring.head
    , &client->shm->client_ring.tail
    , &client->shm->client_ring.producers
    , &client->shm->client_ring.waiters
  );
}

void bam_client_post(bam_client * restrict client, bamipc_message * restrict msg)
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

bamipc_message * bam_client_acquire(bam_client * restrict client)
{
  bamipc_message *msg;
  msg = bamipc_acquire(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , &client->shm->server_ring.tail
  );

  return msg;
}

void bam_client_consume(bam_client * restrict client, bamipc_message * restrict msg)
{
  bamipc_consume(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
    , &client->shm->server_ring.waiters
  );
}

void bam_client_release(bam_client * restrict client, bamipc_message * restrict msg)
{
  bamipc_release(
      client->shm->server_ring.pages
    , &client->shm->server_ring.head
    , msg
  );
}
