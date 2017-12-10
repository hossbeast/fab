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
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/config.h"

#include "monitor_thread.h"
#include "logging.h"
#include "params.h"

#define restrict __restrict

#if 0
void sigdumper(char * name);
void sigdumper(char * name)
{
  sigset_t sigs;
  sigset_t set;
  sigemptyset(&sigs);
  pthread_sigmask(SIG_BLOCK, &sigs, &set);

  printf("%10s (%d) unblocked [", name, gettid());
  int x;
  for(x = 1; x <= SIGRTMAX; x++)
  {
    if(!sigismember(&set, x))
      printf(" %d", x);
  }
  printf(" ]\n");

  int rv = sigpending(&sigs);
  if(rv != 0)
    printf("SIGPENDING FAILED\n");

  printf("%10s (%d) pending [", name, gettid());
  for(x = 1; x <= SIGRTMAX; x++)
  {
    if(sigismember(&sigs, x))
      printf(" %d", x);
  }
  printf(" ]\n");
}
#endif

xapi monitor_thread()
{
  enter;

  sigset_t sigs;
  siginfo_t info;

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
  sigaddset(&sigs, FABIPC_SIGSCH);

  fatal(sigutil_wait, &sigs, &info);

  // teardown other threads
  g_params.shutdown = 1;
  while(g_params.thread_count)
  {
    fatal(uxtgkill, 0, g_params.pid, g_params.thread_server, FABIPC_SIGSCH);
    fatal(uxtgkill, 0, g_params.pid, g_params.thread_notify, FABIPC_SIGINTR);
    fatal(uxtgkill, 0, g_params.pid, g_params.thread_sweeper, FABIPC_SIGINTR);
    fatal(sigutil_wait, &sigs, 0);
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}
