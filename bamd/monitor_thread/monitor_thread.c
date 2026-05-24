/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>

#include "types.h"

#include "xlinux/xsignal.h"
#include "bam/sigutil.h"

#include "monitor_thread.h"
#include "channel_thread.h"
#include "plugin_thread.h"
#include "params.h"
#include "threads.h"
#include "rcu_list.h"
#include "times.h"

void monitor_thread()
{
  sigset_t sigs = {};
  siginfo_t info = {};
  channel_thread *channel;
  plugin_thread *plugin;
  struct timespec interval = {};
  rcu_thread rcu_self = {};

  // signals to be handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGTERM);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGUSR1);

  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(100);

  rcu_enter(&rcu_self);
  while(g_params.thread_count)
  {
    rcu_quiesce(&rcu_self);
    if(sigutil_timedwait(&sigs, &info, &interval) == EAGAIN) {
      continue;
    }

    if(info.si_signo != SIGUSR1) {
      g_params.shutdown = true;
    }

    if(!g_params.shutdown) {
      continue;
    }

    /* cascade signals to everyone */
    uxtgkill(g_params.pid, g_params.thread_channel_server, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_notify, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_sweeper, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_run, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_timeout, SIGUSR1);

    /* client threads */
    rcu_list_foreach(&channel_thread_list, channel, stk) {
      uxtgkill(g_params.pid, channel->tid, SIGUSR1);
    }

    /* plugin threads */
    rcu_list_foreach(&plugin_thread_list, plugin, stk) {
      uxtgkill(g_params.pid, plugin->tid, SIGUSR1);
    }
  }

  rcu_leave(&rcu_self);
}
