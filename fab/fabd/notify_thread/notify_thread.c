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

#include "logger/config.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/map.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xinotify.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xtime.h"
#include "value/writer.h"

#include "notify_thread.h"
#include "fsent.h"
#include "logging.h"
#include "params.h"
#include "sweeper_thread.h"

#include "atomics.h"
#include "threads.h"
#include "times.h"
#include "locks.h"

uint16_t notify_thread_epoch;

static int in_fd = -1;

/* the map is protected by the lock */
static struct futexlock in_lock;
static map *in_fsent_by_wd;

/* inotify event buffer */
static char evbuf[4096];

#include "zbuffer.h"
#include "inotify_mask.h"

static xapi notify_thread()
{
  enter;

  sigset_t sigs;
  fsent *n;
  moria_vertex *v, *c;
  const char *label = 0;
  uint16_t label_len = 0;
  uint16_t evbuf_off;
  uint16_t evbuf_len;
  ssize_t r;
  struct inotify_event *ev;
  struct timespec interval;

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(100);

  /* current offset into evbuf */
  evbuf_off = 0;

  /* length of data in evbuf */
  evbuf_len = 0;
  while(!g_params.shutdown)
  {
    if(evbuf_off < evbuf_len)
    {
      ev = (void*)evbuf + evbuf_off;

      if(ev->mask & IN_IGNORED) {
        goto next;
      }

      if(ev->name[0] == '.') {
        /* ignore dotfiles for now */
        goto next;
      }

      futexlock_acquire(&in_lock);
      n = map_get(in_fsent_by_wd, MM(ev->wd));
      futexlock_release(&in_lock);

      if(n == 0) {
        goto next;
      }

      v = &n->vertex;
      c = 0;

      if(ev->mask & (IN_MOVE_SELF | IN_DELETE_SELF))
      {
        /* only applies to the directory itself */
        label = 0;
        label_len = 0;
      }
      else
      {
        label = ev->name;
        label_len = strlen(ev->name);
        if((c = moria_vertex_downw(v, label, label_len)))
        {
          n = containerof(c, fsent, vertex);
          label = 0;
          label_len = 0;
        }
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
      evbuf_off += sizeof(*ev) + ev->len;
      continue;
    }

    fatal(uxread, in_fd, evbuf, sizeof(evbuf), &r);
    if(r == 0)
    {
      notify_thread_epoch++;
      fatal(uxclock_nanosleep, 0, CLOCK_MONOTONIC, 0, &interval, 0);
    }
    else
    {
      evbuf_len = r;
      evbuf_off = 0;
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

  tid = g_params.thread_notify = gettid();
  logger_set_thread_name("notify");
  logger_set_thread_categories(L_NOTIFY);
  fatal(notify_thread, arg);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("thread", "notify");
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi notify_thread_setup()
{
  enter;

  fatal(xinotify_init, &in_fd, IN_NONBLOCK);
  fatal(map_create, &in_fsent_by_wd);

  finally : coda;
}

xapi notify_thread_cleanup()
{
  enter;

  fatal(ixclose, &in_fd);
  fatal(map_xfree, in_fsent_by_wd);

  finally : coda;
}

xapi notify_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, notify_thread_jump, 0)) != 0)
  {
    atomic_fetch_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

xapi notify_thread_add_watch(fsent * n)
{
  enter;

  char space[512];
  uint32_t mask = 0;
  struct futexlock *lockp = 0;

  RUNTIME_ASSERT(n->wd == -1);

  /* events for the directory itself */
  mask |= IN_DELETE_SELF | IN_MOVE_SELF;

  /* events for files/directories in the directory */
  mask |= IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;

  mask |= IN_ONLYDIR;       /* ENOTDIR if the path is not a directory */
  mask |= IN_EXCL_UNLINK;   /* ignore events for files after they are unlinked */

  fsent_absolute_path_znload(space, sizeof(space), n);
  fatal(xinotify_add_watch, &n->wd, in_fd, space, mask);

  lockp = futexlock_acquire(&in_lock);
  fatal(map_put, in_fsent_by_wd, MM(n->wd), n, 0);
  futexlock_release(&in_lock);
  lockp = 0;

  if(log_would(L_NOTIFY))
  {
    narrator * N;
    fatal(log_start, L_NOTIFY, &N);
    xsayf("%8s ", "watch");
    fatal(fsent_absolute_path_say, n, N);
    fatal(log_finish);
  }

finally:
  if(lockp) {
    futexlock_release(lockp);
  }
coda;
}

xapi notify_thread_rm_watch(fsent * n)
{
  enter;

  /* there should be code here */

  finally : coda;
}
