/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>

#include "types.h"

#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "valyria/llist.h"
#include "bam/ipc.h"
#include "bam/sigutil.h"

#include "monitor_thread.h"
#include "handler.h"
#include "params.h"
#include "threads.h"
#include "rcu_list.h"

void monitor_thread()
{
  sigset_t sigs;
  siginfo_t info;
  handler_context * handler;
  struct timespec interval;
  int r;
  rcu_thread rcu_self = { };

  // signals to be handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGTERM);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGUSR1);
  interval.tv_sec = 0;
  interval.tv_nsec = 125000000;

  rcu_register(&rcu_self);
  while(g_params.thread_count)
  {
    rcu_quiesce(&rcu_self);
    sigutil_timedwait(&r, &sigs, &info, &interval);
    if(r == EAGAIN) {
      continue;
    }

    if(info.si_signo != SIGUSR1) {
      g_params.shutdown = true;
    }

    if(!g_params.shutdown) {
      continue;
    }

    uxtgkill(g_params.pid, g_params.thread_server, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_notify, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_sweeper, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1);

    rcu_list_foreach(&g_handlers, handler, stk) {
      uxtgkill(g_params.pid, handler->tid, SIGUSR1);
    }
  }

  rcu_unregister(&rcu_self);
}
