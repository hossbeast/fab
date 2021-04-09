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

#include "xapi.h"
#include "types.h"

#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "valyria/llist.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/config.h"

#include "monitor_thread.h"
#include "handler.h"
#include "logging.h"
#include "params.h"
#include "threads.h"
#include "rcu_list.h"

xapi monitor_thread()
{
  enter;

  sigset_t sigs;
  siginfo_t info;
  handler_context * handler;
  struct timespec interval;
  int r;
  rcu_thread rcu_self = { };

  logger_set_thread_name("monitor");
  logger_set_thread_categories(L_MONITOR);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

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
    fatal(sigutil_timedwait, &r, &sigs, &info, &interval);
    if(r == EAGAIN) {
      continue;
    }

    if(info.si_signo != SIGUSR1) {
      g_params.shutdown = true;
    }

    if(!g_params.shutdown) {
      continue;
    }

    fatal(uxtgkill, 0, g_params.pid, g_params.thread_server, SIGUSR1);
    fatal(uxtgkill, 0, g_params.pid, g_params.thread_notify, SIGUSR1);
    fatal(uxtgkill, 0, g_params.pid, g_params.thread_sweeper, SIGUSR1);
    fatal(uxtgkill, 0, g_params.pid, g_params.thread_run, SIGUSR1);

    rcu_list_foreach(&g_handlers, handler, stk) {
      fatal(uxtgkill, 0, g_params.pid, handler->tid, SIGUSR1);
    }
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  rcu_unregister(&rcu_self);
coda;
}
