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

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/exit.h"
#include "xapi/calltree.h"
#include "valyria/set.h"

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/stream.h"
#include "logger/config.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xshm.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "moria/graph.h"

#include "logger/arguments.h"

#include "server_thread.internal.h"
#include "handler_thread.h"
#include "config.h"
#include "errtab/FABD.errtab.h"
#include "logging.h"
#include "module.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "config.internal.h"
#include "walker.internal.h"
#include "buildplan.h"
#include "channel.h"
#include "goals.h"
#include "build_thread.internal.h"
#include "rule.h"
#include "match.h"
#include "path.h"
#include "pattern.h"
#include "node_operations.h"
#include "formula.h"

#include "common/attrs.h"
#include "macros.h"
#include "atomics.h"
#include "threads.h"

bool g_server_autorun;

#if DEVEL
bool g_server_no_initial_client;      // no client right at startup
fabipc_channel *g_server_initial_channel;
#endif

static xapi server_thread()
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGRTMIN);

#if DEVEL
  if(g_server_no_initial_client)
  {
    fatal(handler_thread_launch, 0, 0, false);
  }
#endif

  while(!g_params.shutdown)
  {
    fatal(sigutil_wait, &sigs, &siginfo);
    if(g_params.shutdown) {
      break;
    }

    /* signal from monitor thread */
    if(siginfo.si_signo == SIGUSR1)
    {
      continue;
    }

#if DEVEL
    g_logging_skip_reconfigure = false;
#endif

    fatal(handler_thread_launch, siginfo.si_pid, (intptr_t)siginfo.si_value.sival_ptr, false);
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * server_thread_jump(void * arg)
{
  enter;

  xapi R;

  g_tid = g_params.thread_server = gettid();
  logger_set_thread_name("server");
  logger_set_thread_categories(L_SERVER);
  fatal(server_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/server");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi server_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, server_thread_jump, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
