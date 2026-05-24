/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#include "bam/sigutil.h"
#include "xlinux/xpthread.h"

#include "channel_server_thread.h"
#include "channel_thread.h"
#include "params.h"
#include "channel.h"

#include "atomics.h"
#include "threads.h"

static void channel_server_thread()
{
  sigset_t sigs;
  siginfo_t siginfo;

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGRTMIN);

  while(!g_params.shutdown)
  {
    sigutil_wait(&sigs, &siginfo);
    if(g_params.shutdown) {
      break;
    }

    /* signal from monitor thread */
    if(siginfo.si_signo == SIGUSR1) {
      continue;
    }

    channel_thread_launch(siginfo.si_pid, (intptr_t)siginfo.si_value.sival_ptr);
  }
}

static void *channel_server_thread_jump(void * arg)
{
  tid = g_params.thread_channel_server = gettid();
  errnop = &errno;

  channel_server_thread();

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

void channel_server_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, channel_server_thread_jump, 0);

  pthread_attr_destroy(&attr);
}
