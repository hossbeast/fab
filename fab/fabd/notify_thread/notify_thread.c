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
#include <string.h>

#include "xapi.h"
#include "types.h"

#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/config.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "valyria/map.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xdirent.h"
#include "xlinux/xepoll.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xinotify.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xepoll.h"

#include "notify_thread.h"
#include "params.h"
#include "node.h"
#include "sweeper_thread.h"
#include "path.h"
#include "inotify_mask.h"
#include "logging.h"

#include "atomics.h"
#include "macros.h"
#include "threads.h"

uint16_t notify_thread_epoch;

static int in_fd = -1;

static xapi notify_thread()
{
  enter;

  sigset_t sigs;
  char buffer[4096] = {};
  int epfd = -1;
  struct epoll_event event;
  node *n;
  vertex *v;
  const char *label = 0;
  uint16_t label_len = 0;
  ssize_t r;
  size_t o;
  int rv;

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // setup epoll
  fatal(xepoll_create, &epfd);
  memset(&event, 0, sizeof(event));
  event.events = EPOLLIN | EPOLLPRI | EPOLLERR;
  fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, in_fd, &event);

  r = 0;
  while(!g_params.shutdown)
  {
    if(r == 0)
    {
      fatal(uxepoll_pwait, &rv, epfd, &event, 1, -1, &sigs);
      if(rv < 0)
        continue;

      notify_thread_epoch++;
    }

    fatal(uxread, in_fd, buffer, sizeof(buffer), &r);

    o = 0;
    while(o < r)
    {
      struct inotify_event * ev = (void*)buffer + o;

      if(ev->mask & IN_IGNORED)
        goto next;

      if(ev->name[0] == '.') {
        /* ignore dotfiles for now */
        goto next;
      }

      if((n = map_get(g_nodes_by_wd, MM(ev->wd))) == 0) {
        logs(L_WARN, "unknown event");
        goto next;
      }

      v = vertex_containerof(n);

      if(ev->mask & (IN_MOVE_SELF | IN_DELETE_SELF))
      {
        /* only applies to the directory itself */
        label = 0;
      }
      else
      {
        label = ev->name;
        label_len = strlen(ev->name);

        if((v = vertex_downw(v, label, label_len))) {
          n = vertex_value(v);
          label = 0;
        }
      }

      if(n->notify_state != NOTIFY_MONITOR)
      {
        /* should only ever be set for file nodes */
        RUNTIME_ASSERT(label == 0);
      }

      if(n->notify_state == NOTIFY_SUPPRESS)
      {
        goto next;
      }
      else if(n->notify_state == NOTIFY_EXPIRING)
      {
        if(n->notify_epoch == notify_thread_epoch)
        {
          goto next;
        }

        n->notify_state = NOTIFY_MONITOR;
      }

      fatal(sweeper_thread_enqueue, n, ev->mask, label, label_len);

next:
      o += sizeof(*ev) + ev->len;
    }
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  fatal(ixclose, &in_fd);
coda;
}

static void * notify_thread_jump(void * arg)
{
  enter;

  xapi R;

  g_tid = g_params.thread_notify = gettid();
  logger_set_thread_name("notify");
  logger_set_thread_categories(L_NOTIFY);
  fatal(notify_thread, arg);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd/notify");
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

xapi notify_thread_setup()
{
  xproxy(xinotify_init, &in_fd, IN_NONBLOCK);
}

xapi notify_thread_cleanup()
{
  xproxy(ixclose, &in_fd);
}

xapi notify_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, notify_thread_jump, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

xapi notify_thread_watch(node * n)
{
  enter;

  char space[512];
  uint32_t mask = 0;

  RUNTIME_ASSERT(n->wd == -1);

  /* events for the directory itself */
  mask |= IN_DELETE_SELF | IN_MOVE_SELF;

  /* events for files/directories in the directory */
  mask |= IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;

  mask |= IN_ONLYDIR;       /* ENOTDIR if the path is not a directory */
  mask |= IN_EXCL_UNLINK;   /* ignore events for files after they are unlinked */

  node_absolute_path_znload(space, sizeof(space), n);
  fatal(xinotify_add_watch, &n->wd, in_fd, space, mask);
  fatal(map_put, g_nodes_by_wd, MM(n->wd), n, 0);

  if(log_would(L_NOTIFY))
  {
    narrator * N;
    fatal(log_start, L_NOTIFY, &N);
    xsayf("%8s ", "watch");
    fatal(node_absolute_path_say, n, N);
    fatal(log_finish);
  }

  finally : coda;
}
