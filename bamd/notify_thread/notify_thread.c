/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/syscall.h>
#include <errno.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xinotify.h"
#include "xlinux/xpthread.h"
#include "xlinux/xunistd.h"
#include "xlinux/xtime.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "valyria/map.h"

#include "notify_thread.h"
#include "fsent.h"
#include "params.h"
#include "sweeper_thread.h"
#include "stats.h"
#include "vertex.h"
#include "probes.h"
#include "filesystem.h"
#include "notify.h"

#include "atomics.h"
#include "threads.h"
#include "times.h"
#include "locks.h"

uint16_t notify_thread_epoch;
map *notify_thread_fsent_by_in_wd;

static int in_fd = -1;

/* the map is protected by the lock */
static struct futexlock in_lock;

/* inotify event buffer */
static char evbuf[4096];

static void notify_thread()
{
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
        /* just ignore dotfiles for now */
        goto next;
      }

      futexlock_acquire(&in_lock);
      n = map_get(notify_thread_fsent_by_in_wd, MM(ev->wd));
      futexlock_release(&in_lock);

      if(n == 0) {
        goto next;
      }

      notify_thread_probe(ev, n);

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
        if(n->in_epoch == notify_thread_epoch)
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
  errnop = &errno;

  notify_thread();

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

void notify_thread_setup()
{
  in_fd = xinotify_init(IN_NONBLOCK);
  map_create(&notify_thread_fsent_by_in_wd);
}

void notify_thread_cleanup()
{
  ixclose(&in_fd);
  map_free(notify_thread_fsent_by_in_wd);
}

void notify_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, notify_thread_jump, 0);

  pthread_attr_destroy(&attr);
}

void notify_thread_add_watch(fsent * n)
{
  char space[PATH_MAX];
  uint32_t mask = 0;

  notify_stats.put++;

  /* events for the directory itself */
  mask |= IN_DELETE_SELF | IN_MOVE_SELF;

  /* events for files/directories in the directory */
  mask |= IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;

  /* ignore events for files after they are unlinked */
  mask |= IN_EXCL_UNLINK;

  if(vertex_type_get(&n->vertex) == VERTEX_DIR) {
    mask |= IN_ONLYDIR;     /* ENOTDIR if the path is not a directory */
  }

  fsent_absolute_path_cat(space, sizeof(space), n);
  n->in_wd = inotify_add_watch(in_fd, space, mask);
  if(n->in_wd < 0) {
    n->in_wd = -1;
    if(errno != ENOENT) {
      dprintf(2, "inotify_add_watch(%s, 0x%x) failed : %s\n", space, mask, strerror(errno));
      exit(1);
    }
    return;
  }

  futexlock_acquire(&in_lock);
  map_put(notify_thread_fsent_by_in_wd, MM(n->in_wd), n, 0);
  futexlock_release(&in_lock);
}

void notify_thread_rm_watch(fsent * n)
{
  int r;

  notify_stats.del++;

  futexlock_acquire(&in_lock);
  map_delete(notify_thread_fsent_by_in_wd, MM(n->in_wd));
  futexlock_release(&in_lock);

  r = inotify_rm_watch(in_fd, n->in_wd);
  if(r != 0)
  {
    /* when the directory is deleted, the inotify watch is cleaned up automatically, and
     * EINVAL is the result when attempting to remove it after the fact */
    RUNTIME_ASSERT(errno == EINVAL);
  }

  /* watch counting */
  n->in_wd = -1;
}
