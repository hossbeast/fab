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
#include "xlinux/xstdlib.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "logger/config.h"
#include "valyria/map.h"
#include "valyria/llist.h"
#include "narrator.h"

#include "sweeper_thread.h"
#include "server_thread.h"
#include "filesystem.h"
#include "inotify_mask.h"
#include "logging.h"
#include "node.h"
#include "node_operations.h"
#include "params.h"
#include "walker.h"
#include "module.h"
#include "path.h"

#include "common/spinlock.h"
#include "common/atomic.h"
#include "macros.h"
#include "zbuffer.h"

#define SWEEP_INTERVAL_SEC 1

static llist event_queue;
static llist child_event_freelist;
static int event_queue_lock;
static uint32_t event_era;

static xapi process_event(sweeper_event * restrict ev, graph_invalidation_context * restrict invalidation)
{
  enter;

  char path[512];
  vertex *v;
  edge *ue;
  node *n;
  sweeper_child_event *cev = 0;
  size_t z;
  narrator *N;

  if(ev->kind == SWEEPER_EVENT_SELF)
  {
    n = containerof(ev, node, pending);
  }
  else if(ev->kind == SWEEPER_EVENT_CHILD)
  {
    cev = containerof(ev, typeof(*cev), sweep_event);
    n = cev->parent;
  }
  else
  {
    RUNTIME_ABORT();
  }

  if(log_would(L_FSEVENT))
  {
    if((ev->mask & IN_DELETE) && ev->kind == SWEEPER_EVENT_CHILD)
    {
      /* entity created then subsequently deleted during the epoch - no-op */
    }
    else
    {
      const char *event;
      if(ev->mask & IN_DELETE)
      {
        /* entity was deleted */
        event = "DELETE";
      }
      else if(ev->kind == SWEEPER_EVENT_CHILD)
      {
        event = "CREATE";
      }
      else
      {
        event = "MODIFY";
      }

      fatal(log_start, L_FSEVENT, &N);
      fatal(node_project_relative_path_say, n, N);
      if(cev)
      {
        xsays("//");
        xsayw(cev->name, cev->name_len);
      }
      xsays(" ");
      xsays(event);
      xsayf(" era %"PRIu32, event_era);
      fatal(log_finish);
    }
  }

  if((ev->mask & IN_DELETE) && ev->kind == SWEEPER_EVENT_CHILD)
  {
    /* entity created then subsequently deleted during the epoch - no-op */
  }
  else if(ev->mask & IN_DELETE)
  {
    /* entity was deleted */
    RUNTIME_ASSERT(ev->kind == SWEEPER_EVENT_SELF);

    v = vertex_containerof(n);
    if((ue = v->up_identity) == 0)
    {
      /* deleted the root node ?? */
      goto XAPI_FINALLY;
    }

    fatal(node_edge_disconnect, ue, invalidation);
  }
  else if(ev->kind == SWEEPER_EVENT_CHILD)
  {
    n = 0;

    /* entity was created */
    if(ev->mask & IN_ISDIR)
    {
      z = node_get_absolute_path(cev->parent, path, sizeof(path));
      z += znloadc(path + z, sizeof(path) - z, '/');
      z += znloadw(path + z, sizeof(path) - z, cev->name, cev->name_len);
      path[z] = 0;

      /* traverse the directory (recursively) and attach at cev->parent */
      fatal(walker_descend, 0, 0, cev->parent, path, 0, invalidation);
    }
    else
    {
      fatal(node_createw, &n, VERTEX_FILETYPE_REG | VERTEX_OK, 0, 0, cev->name, cev->name_len);
      fatal(node_connect_fs, cev->parent, n, EDGE_TYPE_FS, invalidation, 0, 0);
    }
  }
  else
  {
    /* entity was modified */
    fatal(node_invalidate, n, invalidation);
  }

  finally : coda;
}

static xapi sweep()
{
  enter;

  graph_invalidation_context invalidation = { 0 };
  sweeper_event *ev;
  sweeper_child_event *ce;
  llist *cursor;

  fatal(graph_invalidation_begin, &invalidation);

  llist_foreach_safe(&event_queue, ev, lln, cursor) {
    /* subsequent events have not yet expired */
    if((event_era - ev->era) < 2) {
      break;
    }

    fatal(process_event, ev, &invalidation);

    llist_delete(ev, lln);
    if(ev->kind == SWEEPER_EVENT_SELF) {
      ev->era = 0;
    } else if(ev->kind == SWEEPER_EVENT_CHILD) {
      llist_append(&child_event_freelist, ev, lln);
      ce = containerof(ev, sweeper_child_event, sweep_event);
      rbtree_delete(&ce->parent->pending_child, ce, rbn);
    }
  }

finally:
  graph_invalidation_end(&invalidation);
coda;
}

static xapi sweeper_thread()
{
  enter;

  sigset_t sigs;
  struct timespec start;
  struct timespec next;
  typeof(start.tv_sec) iteration;
  sigset_t orig;

  g_params.thread_sweeper = gettid();

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, FABIPC_SIGINTR);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  fatal(xclock_gettime, CLOCK_MONOTONIC, &start);
  memcpy(&next, &start, sizeof(next));

  for(iteration = 1; !g_params.shutdown; iteration++)
  {
    spinlock_engage(&event_queue_lock);
    event_era++;

    if(!llist_empty(&event_queue))
    {
      fatal(sweep);
      if(llist_empty(&event_queue) && g_server_autorun)
      {
        fatal(sigutil_tgkill, g_params.pid, g_params.thread_server, FABIPC_SIGINTR);
      }
    }

    spinlock_release(&event_queue_lock);

    /* sleep */
    next.tv_sec = start.tv_sec + (iteration * SWEEP_INTERVAL_SEC);
    fatal(xpthread_sigmask, SIG_SETMASK, &sigs, &orig);
    fatal(uxclock_nanosleep, 0, CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
    fatal(xpthread_sigmask, SIG_SETMASK, &orig, 0);
  }

finally:
  spinlock_release(&event_queue_lock);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * sweeper_thread_main(void * arg)
{
  enter;

  xapi R;
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
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, FABIPC_SIGSCH);
  return 0;
}

//
// public
//

struct event_rbn_key {
  const char *name;
  uint8_t name_len;
};

static int event_rbn_key_cmp(void * _key, const rbnode * _n)
{
  const struct event_rbn_key *key = _key;
  const sweeper_child_event *n = containerof(_n, typeof(*n), rbn);

  return memncmp(key->name, key->name_len, n->name, n->name_len);
}

xapi sweeper_thread_enqueue(node *n, uint32_t mask, const char * restrict name, uint16_t namel)
{
  enter;

  sweeper_event *e;
  sweeper_child_event *ce;
  rbnode *rbn;
  rbtree_search_context search_ctx;

  spinlock_engage(&event_queue_lock);

  if(name)
  {
    /* lookup the event record for this name, under this directory */
    struct event_rbn_key key = (typeof(key)) {
        .name = name
      , .name_len = namel
    };
    if((rbn = rbtree_search(&n->pending_child, &search_ctx, &key, event_rbn_key_cmp)))
    {
      ce = containerof(rbn, typeof(*ce), rbn);
    }
    else
    {
      if((ce = llist_shift(&child_event_freelist, typeof(*ce), sweep_event.lln)) == 0)
      {
        fatal(xmalloc, &ce, sizeof(*ce));
        ce->sweep_event.kind = SWEEPER_EVENT_CHILD;
        llist_init_node(&ce->sweep_event.lln);
      }
      else
      {
        ce->sweep_event.mask = 0;
      }

      memcpy(ce->name, name, namel);
      ce->name_len = namel;
      ce->parent = n;

      rbtree_insert_node(&n->pending_child, &search_ctx, &ce->rbn);
    }
    e = &ce->sweep_event;
  }
  else
  {
    e = &n->pending;
    if(e->era == 0) {
      e->mask = 0;
    }
  }

  /* push to the end */
  llist_delete(e, lln);
  llist_append(&event_queue, e, lln);

  /* coalesce events */
  if(mask & IN_CREATE) {
    e->mask &= ~(IN_DELETE | IN_MOVED_FROM);
  }

  e->mask |= mask;

  /* refresh timer */
  e->era = event_era;

finally:
  spinlock_release(&event_queue_lock);
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

  finally : coda;
}

static void __attribute__((constructor)) setup()
{
  llist_init_node(&event_queue);
  llist_init_node(&child_event_freelist);
}

xapi sweeper_thread_cleanup()
{
  enter;

  sweeper_child_event *ce;
  llist *cursor;

  llist_foreach_safe(&child_event_freelist, ce, sweep_event.lln, cursor) {
    free(ce);
  }

  finally : coda;
}
