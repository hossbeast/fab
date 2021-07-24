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

#include "narrator.h"
#include "valyria/map.h"
#include "xlinux/xinotify.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xtime.h"

#include "notify_thread.h"
#include "fsent.h"
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

static void notify_thread()
{
  sigset_t sigs;
  fsent *n;
  moria_vertex *v;
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

      if(ev->mask & (IN_MOVE_SELF | IN_DELETE_SELF))
      {
        /* only applies to the directory itself */
        label = 0;
      }
      else
      {
        label = ev->name;
        label_len = strlen(ev->name);

        if((v = moria_vertex_downw(v, label, label_len))) {
          n = containerof(v, fsent, vertex);
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

      sweeper_thread_enqueue(n, ev->mask, label, label_len);

next:
      evbuf_off += sizeof(*ev) + ev->len;
      continue;
    }

    r = uxread(in_fd, evbuf, sizeof(evbuf));
    if(r == 0)
    {
      notify_thread_epoch++;
      uxclock_nanosleep(CLOCK_MONOTONIC, 0, &interval, 0);
    }
    else
    {
      evbuf_len = r;
      evbuf_off = 0;
    }
  }

  ixclose(&in_fd);
}

static void * notify_thread_jump(void * arg)
{
  tid = g_params.thread_notify = gettid();

  notify_thread(arg);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

void notify_thread_setup()
{
  in_fd = xinotify_init(IN_NONBLOCK);
  map_create(&in_fsent_by_wd);
}

void notify_thread_cleanup()
{
  ixclose(&in_fd);
  map_xfree(in_fsent_by_wd);
}

void notify_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, notify_thread_jump, 0);

  pthread_attr_destroy(&attr);
}

void notify_thread_add_watch(fsent * n)
{
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
  n->wd = xinotify_add_watch(in_fd, space, mask);

  lockp = futexlock_acquire(&in_lock);
  map_put(in_fsent_by_wd, MM(n->wd), n, 0);
  futexlock_release(&in_lock);
  lockp = 0;

  futexlock_release(lockp);
}

void notify_thread_rm_watch(fsent * n)
{
  /* there should be code here */
}
