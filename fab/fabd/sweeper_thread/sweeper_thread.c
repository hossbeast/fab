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

#include <string.h>
#include <sys/syscall.h>
#include <sys/inotify.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xstat.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xunistd.h"
#include "xlinux/xtime.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/config.h"
#include "valyria/array.h"
#include "valyria/map.h"
#include "narrator.h"

#include "sweeper_thread.h"
#include "filesystem.h"
#include "inotify_mask.h"
#include "logging.h"
#include "node.h"
#include "node_operations.h"
#include "params.h"
#include "walker.h"
#include "path.h"

#include "spinlock.h"
#include "atomic.h"
#include "macros.h"
#include "zbuffer.h"

#define restrict __restrict

struct event
{
  int wd;
  uint32_t mask;
  char name[128];
  size_t namel;
};

static array * queue;
static int queue_lock;

static xapi sweeper_thread()
{
  enter;

  char space[512];
  sigset_t sigs;
  struct timespec start;
  struct timespec next;
  int iteration = 0;
  size_t queue_len;
  node * n;

  g_params.thread_sweeper = gettid();

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, FABIPC_SIGINTR);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  fatal(xclock_gettime, CLOCK_MONOTONIC, &start);
  memcpy(&next, &start, sizeof(next));

  queue_len = 0;
  while(!g_params.shutdown)
  {
    spinlock_engage(&queue_lock);

    if(queue->l && queue->l == queue_len)
    {
      // open a traversal
      int traversal = node_traversal_begin();

      int x;
      for(x = 0; x < queue->l; x++)
      {
        struct event * ev = array_get(queue, x);
        node * parent = map_get(g_nodes_by_wd, MM(ev->wd));

#if 0
narrator * N = g_narrator_stdout;
sayf("EVENT %s/%s ", parent->name->name, ev->name);
fatal(inotify_mask_say, ev->mask, g_narrator_stdout);
sayf("\n");
#endif

        if(ev->mask & (IN_CREATE | IN_MOVED_TO))
        {
          if(ev->mask & IN_ISDIR)
          {
            size_t pathl = node_get_path(parent, space, sizeof(space));
            pathl += znloads(space + pathl, sizeof(space) - pathl, "/");
            pathl += znloadw(space + pathl, sizeof(space) - pathl, ev->name, ev->namel);

            n = 0;
            fatal(walker_walk, &n, parent, space);
          }
          else
          {
            n = 0;
            fatal(node_createw, &n, NODE_FS_TYPE_FILE, parent->fs, ev->name, ev->namel);
            fatal(node_connect, parent, n);
          }
        }

        else if(ev->mask & (IN_DELETE | IN_MOVED_FROM))
        {
          edge * e = vertex_descend_edge(vertex_containerof(parent), ev->name);
          if(e)
            fatal(node_disintegrate_fs, e, traversal);
        }

        else if(ev->mask & IN_MODIFY)
        {
          vertex * v = vertex_descend(vertex_containerof(parent), ev->name);
          fatal(node_invalidate, vertex_value(v), traversal);
        }
      }

      queue->l = 0;
      fatal(node_dump);
    }

    queue_len = queue->l;
    spinlock_release(&queue_lock);

    next.tv_sec = start.tv_sec + (++iteration * 1);
    fatal(uxclock_nanosleep, 0, CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * sweeper_thread_main(void * arg)
{
  enter;

  xapi R;
  char space[4096];

  logger_set_thread_name("sweeper");
  logger_set_thread_categories(L_SWEEPER);
  fatal(sweeper_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd/sweeper");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    xapi_trace_full(space, sizeof(space));
#else
    xapi_trace_pithy(space, sizeof(space));
#endif
    logf(L_ERROR, "\n%s", space);
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, FABIPC_SIGSCH);
  return 0;
}

//
// public
//

xapi sweeper_thread_enqueue(int wd, uint32_t mask, const char * restrict name, size_t namel)
{
  enter;

  struct event * e;
  size_t len;

  spinlock_engage(&queue_lock);
  fatal(array_push, queue, &e);
  e->wd = wd;
  e->mask = mask;
  len = MIN(sizeof(e->name) - 1, namel);
  memcpy(e->name, name, len);
  e->name[len] = 0;
  e->namel = len;

finally:
  spinlock_release(&queue_lock);
coda;
}

xapi sweeper_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, sweeper_thread_main, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

xapi sweeper_thread_setup()
{
  enter;

  fatal(array_create, &queue, sizeof(struct event));

  finally : coda;
}

xapi sweeper_thread_cleanup()
{
  enter;

  fatal(array_xfree, queue);

  finally : coda;
}
