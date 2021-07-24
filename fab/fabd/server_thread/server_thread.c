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

#include <unistd.h>
#include <sys/syscall.h>

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "xlinux/xpthread.h"

#include "server_thread.h"
#include "handler_thread.h"
#include "params.h"
#include "channel.h"

#include "atomics.h"
#include "threads.h"

static void server_thread()
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
    if(siginfo.si_signo == SIGUSR1)
    {
      continue;
    }

    handler_thread_launch(siginfo.si_pid, (intptr_t)siginfo.si_value.sival_ptr);
  }
}

static void * server_thread_jump(void * arg)
{
  tid = g_params.thread_server = gettid();

  server_thread();

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

void server_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, server_thread_jump, 0);

  pthread_attr_destroy(&attr);
}
