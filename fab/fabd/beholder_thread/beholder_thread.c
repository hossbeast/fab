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

#include "fab/events.h"
#include "fab/ipc.h"
#include "logger/config.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xepoll.h"
#include "xlinux/xpthread.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"

#include "beholder_thread.h"
#include "events.h"
#include "handler.h"
#include "logging.h"
#include "params.h"
#include "rcu.h"

#include "atomics.h"
#include "times.h"

int beholder_stdout_rd = -1;
int beholder_stderr_rd = -1;

static xapi process(struct epoll_event *ev, int len)
{
  enter;

  char buf[4096];
  int x;
  ssize_t bytes;
  handler_context *handler;
  fabipc_message *msg;
  int real_fd;
  uint32_t event;

  for(x = 0; x < len; x++)
  {
    fatal(xread, ev[x].data.u32, buf, sizeof(buf), &bytes);
    if(ev[x].data.u32 == beholder_stdout_rd) {
      real_fd = STDOUT_SAVED_REAL;
      event = FABIPC_EVENT_BAMD_STDOUT;
    } else {
      real_fd = STDERR_SAVED_REAL;
      event = FABIPC_EVENT_BAMD_STDERR;
    }

    // propagate to the original fd
    fatal(xwrite, real_fd, buf, bytes, 0);

    // propagate to event subscribers if any
    if(events_would(event, &handler, &msg)) {
      msg->size = bytes;
      memcpy(msg->text, buf, bytes);
      events_publish(handler, msg);
    }
  }

  finally : coda;
}

static xapi beholder_thread()
{
  enter;

  int epfd = -1;
  struct epoll_event ev[8];
  int rv;
  sigset_t sigs;
  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  fatal(xepoll_create, &epfd);

  memset(&ev[0], 0, sizeof(ev[0]));
  ev[0].events = EPOLLIN | EPOLLPRI | EPOLLERR;

  ev[0].data.u32 = beholder_stdout_rd;
  fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, beholder_stdout_rd, &ev[0]);

  ev[0].data.u32 = beholder_stderr_rd;
  fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, beholder_stderr_rd, &ev[0]);

  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);

    /* blocking, with timeout */
    fatal(uxepoll_pwait, &rv, epfd, ev, sizeof(ev) / sizeof(*ev), SEC_AS_MSEC(1), &sigs);
    fatal(process, ev, rv);
  }

  rv = 1;
  while(rv > 0)
  {
    rcu_quiesce(&rcu_self);

    /* non-blocking */
    fatal(uxepoll_pwait, &rv, epfd, ev, sizeof(ev) / sizeof(*ev), 0, &sigs);
    fatal(process, ev, rv);
  }

  /* restore original fds */
  dup2(STDOUT_SAVED_REAL, 1);
  dup2(STDERR_SAVED_REAL, 2);

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
  rcu_unregister(&rcu_self);

  fatal(xclose, epfd);
coda;
}

static void * beholder_thread_jump(void * arg)
{
  enter;

  xapi R;

  tid = g_params.thread_beholder = gettid();
  logger_set_thread_name("beholder");
  logger_set_thread_categories(L_BEHOLDER);

  fatal(beholder_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("thread", "beholder");
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

xapi beholder_thread_setup()
{
  enter;

  finally : coda;
}

xapi beholder_thread_cleanup()
{
  enter;

  fatal(xclose, beholder_stdout_rd);
  fatal(xclose, beholder_stderr_rd);

  finally : coda;
}

xapi beholder_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, beholder_thread_jump, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
