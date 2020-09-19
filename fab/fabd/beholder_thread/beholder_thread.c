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
#include <unistd.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "logger/config.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xepoll.h"
#include "valyria/stack.h"

#include "beholder_thread.h"
#include "handler_thread.h"
#include "channel.h"
#include "epoll_attrs.h"
#include "logging.h"
#include "params.h"
#include "rcu.h"
#include "events.h"

int beholder_stdout_rd = -1;
int beholder_stderr_rd = -1;

static xapi beholder_thread()
{
  enter;

  int epfd = -1;
  struct epoll_event ev;
  int rv;
  sigset_t sigs;
  char buf[4096];
  ssize_t bytes;
  handler_context *handler;
  fabipc_message *msg;
  rcu_thread rcu_self = { 0 };
  int real_fd;
  uint32_t event;
  uint32_t tail;

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  fatal(xepoll_create, &epfd);

  memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN | EPOLLPRI | EPOLLERR;

  ev.data.u32 = beholder_stdout_rd;
  fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, beholder_stdout_rd, &ev);

  ev.data.u32 = beholder_stderr_rd;
  fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, beholder_stderr_rd, &ev);

  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);

    fatal(uxepoll_pwait, &rv, epfd, &ev, 1, 1000, &sigs);
    if(!rv) {
      continue;
    }

    fatal(xread, ev.data.u32, buf, sizeof(buf), &bytes);

    if(ev.data.u32 == beholder_stdout_rd) {
      real_fd = BEHOLDER_STDOUT_REAL;
      event = FABIPC_EVENT_STDOUT;
    } else {
      real_fd = BEHOLDER_STDERR_REAL;
      event = FABIPC_EVENT_STDERR;
    }

    // propagate to the original fd
    fatal(xwrite, real_fd, buf, bytes, 0);

//if(real_fd == BEHOLDER_STDOUT_REAL) {
//  continue;
//}

//if(real_fd == BEHOLDER_STDERR_REAL) {
//  static int ZZ;
//  if(++ZZ > 10) {
//    continue;
//  }
//}

    // propagate to event subscribers if any
    if(events_would(event, &handler, &msg, &tail)) {
      msg->size = bytes;
      memcpy(msg->text, buf, bytes);
      events_publish(handler, msg, tail);
    }
  }

  /* restore original fds */
  dup2(BEHOLDER_STDOUT_REAL, 1);
  dup2(BEHOLDER_STDERR_REAL, 2);

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

  logger_set_thread_name("beholder");
  logger_set_thread_categories(L_BEHOLDER);
  g_params.thread_beholder = gettid();

  fatal(beholder_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/beholder");
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
