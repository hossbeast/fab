/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/syscall.h>
#include <errno.h>
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
#include "xlinux/xfutex.h"
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
#include "lorien/path_normalize.h"
#include "common/hash.h"
#include "common/parseint.h"
#include "common/attrs.h"
#include "common/sha1.h"

#include "client.h"
#include "lockfile.h"
#include "sigutil.h"
#include "ipc.h"
#include "pipe.h"
#include "ring.h"

#include "macros.h"
#include "barriers.h"
#include "atomics.h"
#include "zcat.h"

#if CONFIG_DEVEL
const char *g_bam_client_bamd_path;
const char *g_bam_client_system_config_path;
const char *g_bam_client_user_config_path;
const char *g_bam_client_default_filesystem_invalidation;
const char *g_bam_client_sweeper_period_nsec;
#endif

/* ugo+rwx world read/write/execute */
#define IPCDIR_MODE_DIR   0777

/* ugo+rw  world read/write */
#define IPCDIR_MODE_DATA  0666

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
  sha1_ctx sha;
  uint8_t hash[20];
  path_normalize_context ctx;

  memset(client, 0, sizeof(*client));

  /* canonical path to base */
  xmalloc(&client->basedir_real, PATH_MAX);
  client->basedir_real = xrealpathw(client->basedir_real, basedir, basedir_len);
  client->basedir_real_len = strlen(client->basedir_real);

  /* bamd id is computed over this canonical path */
  sha1_init(&sha);
  sha1_update(&sha, client->basedir_real, client->basedir_real_len);
  sha1_final(hash, &sha);

  /* bamd id encoded as first sixteen bytes as hexascii of this sha */
  zcatf(client->bamd_id, sizeof(client->bamd_id)
    , "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
      "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
    , hash[ 0], hash[ 1], hash[ 2], hash[ 3]
    , hash[ 4], hash[ 5], hash[ 6], hash[ 7]
    , hash[ 8], hash[ 9], hash[10], hash[11]
    , hash[12], hash[13], hash[14], hash[15]
  );

  /* normalized path to base is used by bamd for constructing paths */
  xmalloc(&client->basedir, PATH_MAX);
  memset(&ctx, 0, sizeof(ctx));
  ctx.base = basedir;
  ctx.base_len = basedir_len;
  path_normalize(&ctx, client->basedir, PATH_MAX, basedir, basedir_len);
  client->basedir_len = strlen(client->basedir);

  /* base ipc dir */
  xmalloc(&client->ipcdir_root, ipcdir_len + 1);
  memcpy(client->ipcdir_root, ipcdir, ipcdir_len);
  client->ipcdir_root_len = ipcdir_len;

  // ipc dir for the bamd instance
  len = snprintf(space, sizeof(space), "%.*s/%s", (int)ipcdir_len, ipcdir, client->bamd_id);
  xmalloc(&client->ipcdir, len + 1);
  memcpy(client->ipcdir, space, len);
  client->ipcdir_len = len;

  client->msgid = 2;
  client->lockfd = -1;
  client->bamd_pid = -1;
}

void bam_client_destroy(bam_client * restrict client)
{
  free(client->ipcdir_root);
  free(client->ipcdir);
  free(client->basedir);
  free(client->basedir_real);
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
  bamipc_lockfile_acquire(&client->bamd_pid, &client->lockfd, path);
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
  uxunlinkf("%s/basedir_real", client->ipcdir);
  xsymlinkf("%s", "%s/basedir_real", client->basedir_real, client->ipcdir);

  // cleanup the directory
  uxunlinkf("%s/bamd/exit", client->ipcdir);
  uxunlinkf("%s/bamd/core", client->ipcdir);
  uxunlinkf("%s/bamd/stdout", client->ipcdir);
  uxunlinkf("%s/bamd/stderr", client->ipcdir);
#if CONFIG_DEVEL
  uxunlinkf("%s/bamd/objects", client->ipcdir);
#endif

  // block all signals
  sigfillset(&sigs);
  xpthread_sigmask(SIG_SETMASK, &sigs, &oset);

  if((client->bamd_pid = xfork()) == 0)
  {
    // prepare the command
    i = 0;
    argv[i++] = "bamd";
    argv[i++] = client->bamd_id;

    /* its a hack, but these 100 bytes gives bamd a simple way to record/display the base path */
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

  /* at this point the lockfile, if any, is now held by bamd and no longer needed */
  ixclose(&client->lockfd);

  /* the shm is initialized by the server ahead of time */
}

void bam_client_disconnect(bam_client * restrict client)
{
  if(!client->shm) {
    return;
  }

  client->shm->client_exit = 1;
  bamipc_wake_tail(&client->shm->tx.ring);
}

int bam_client_tx(bam_client * restrict client, struct timespec * restrict timeout, struct bamipc_message ** restrict msg)
{
  return bamipc_tx(&client->shm->tx, timeout, msg);
}

void bam_client_publish(bam_client * restrict client, bamipc_message * restrict msg)
{
  return bamipc_publish(&client->shm->tx, msg);
}

int bam_client_rx(bam_client * restrict client, struct timespec * restrict timeout, bamipc_message ** restrict msg)
{
  return bamipc_rx(&client->shm->rx, timeout, msg);
}

void bam_client_release(bam_client * restrict client, bamipc_message * restrict msg)
{
  bamipc_release(&client->shm->rx, msg);
}
