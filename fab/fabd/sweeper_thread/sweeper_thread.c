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
#include <sys/inotify.h>

#include "xapi.h"

#include "logger.h"
#include "logger/config.h"
#include "narrator.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xpthread.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xtime.h"

#include "sweeper_thread.h"
#include "args.h"
#include "fsedge.h"
#include "fsent.h"
#include "goals.h"
#include "graph.h"
#include "run_thread.h"
#include "logging.h"
#include "params.h"
#include "rcu.h"
#include "walker.h"
#include "zbuffer.h"

#include "locks.h"
#include "atomics.h"
#include "threads.h"
#include "times.h"

static llist event_queue = LLIST_INITIALIZER(event_queue);
static llist child_event_freelist = LLIST_INITIALIZER(child_event_freelist);
static struct spinlock event_queue_lock;
static uint32_t event_era;

static xapi process_event(sweeper_event * restrict ev, graph_invalidation_context * restrict invalidation)
{
  enter;

  char path[512];
  moria_vertex *v;
  moria_edge *ue;
  fsent *n;
  sweeper_child_event *cev = 0;
  size_t z;
  narrator *N;
  const char *event;

  if(ev->kind == SWEEPER_EVENT_SELF)
  {
    n = containerof(ev, fsent, pending);
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

      if(log_would(L_FSEVENT))
      {
        fatal(log_start, L_FSEVENT, &N);
        xsayf(" era %5"PRIu32" %9s ", event_era, event);
        fatal(fsent_project_relative_path_say, n, N);
        if(cev)
        {
          xsays("//");
          xsayw(cev->name, cev->name_len);
        }
        fatal(log_finish);
      }
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

    v = &n->vertex;
    ue = v->up_identity;

    /* deleted the root node ?? */
    RUNTIME_ASSERT(ue);

    fatal(graph_disintegrate, ue, invalidation);
  }
  else if(ev->kind == SWEEPER_EVENT_CHILD)
  {
    n = 0;

    /* entity was created */
    if(ev->mask & IN_ISDIR)
    {
      z = fsent_absolute_path_znload(path, sizeof(path), cev->parent);
      z += znloadc(path + z, sizeof(path) - z, '/');
      z += znloadw(path + z, sizeof(path) - z, cev->name, cev->name_len);
      path[z] = 0;

      /* traverse the directory (recursively) and attach at cev->parent */
      fatal(walker_descend, 0, 0, cev->parent, path, 0, invalidation);
    }
    else
    {
      fatal(fsent_create, &n, VERTEX_TYPE_FSENT, VERTEX_OK, cev->name, cev->name_len);
      fatal(fsedge_connect, cev->parent, n, invalidation);
    }
  }
  else
  {
    /* entity was modified */
    fsent_exists_set(n);
    fatal(fsent_invalidate, n, invalidation);
  }

  finally : coda;
}

static xapi sweep()
{
  enter;

  graph_invalidation_context invalidation = { };
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
  struct timespec deadline;
  sigset_t orig;
  int r;
  uint64_t period;
  rcu_thread rcu_self = { };

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  period = g_args.sweeper_period_nsec;

  fatal(xclock_gettime, CLOCK_MONOTONIC, &deadline);
  timespec_add(&deadline, period);

  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    spinlock_acquire(&event_queue_lock);
    event_era++;

    if(!llist_empty(&event_queue))
    {
      fatal(sweep);
      if(llist_empty(&event_queue) && run_thread_autorun)
      {
        fatal(run_thread_launch, 0, COMMAND_AUTORUN);
      }
    }

    spinlock_release(&event_queue_lock);

    /* sleep */
    fatal(xpthread_sigmask, SIG_SETMASK, &sigs, &orig);
    fatal(uxclock_nanosleep, &r, CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, 0);
    fatal(xpthread_sigmask, SIG_SETMASK, &orig, 0);

    /* only advance the deadline if the sleep was not interrupted */
    if(r) {
      continue;
    }

    timespec_add(&deadline, period);
  }

finally:
  spinlock_release(&event_queue_lock);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  rcu_unregister(&rcu_self);
coda;
}

static void * sweeper_thread_jump(void * arg)
{
  enter;

  xapi R;

  tid = g_params.thread_sweeper = gettid();
  logger_set_thread_name("sweeper");
  logger_set_thread_categories(L_SWEEPER);

  fatal(sweeper_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("thread", "sweeper");
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

//
// public
//

xapi sweeper_thread_enqueue(fsent *n, uint32_t mask, const char * restrict name, uint16_t namel)
{
  enter;

  sweeper_event *e;
  sweeper_child_event *ce;
  rbnode *rbn;
  rbtree_search_context search_ctx;
  struct event_rbn_key key;

  spinlock_acquire(&event_queue_lock);

  if(name)
  {
    /* lookup the event record for this name, under this directory */
    key = (typeof(key)) {
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

  atomic_fetch_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, sweeper_thread_jump, 0)) != 0)
  {
    atomic_fetch_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
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
