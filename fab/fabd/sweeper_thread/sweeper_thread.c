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

#include "narrator.h"
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
#include "params.h"
#include "rcu.h"
#include "walker.h"
#include "zbuffer.h"
#include "probes.h"

#include "locks.h"
#include "atomics.h"
#include "threads.h"
#include "times.h"

static llist event_queue = LLIST_INITIALIZER(event_queue);
static llist child_event_freelist = LLIST_INITIALIZER(child_event_freelist);
static struct spinlock event_queue_lock;
static uint32_t event_era;

static void process_event(sweeper_event * restrict ev, graph_invalidation_context * restrict invalidation)
{
  char path[512];
  moria_vertex *v;
  moria_edge *ue;
  fsent *n;
  sweeper_child_event *cev;
  size_t z;

  if(ev->kind == SWEEPER_EVENT_SELF)
  {
    n = containerof(ev, fsent, pending);
    cev = 0;
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

  if((ev->mask & IN_DELETE) && ev->kind == SWEEPER_EVENT_CHILD)
  {
    /* entity created then subsequently deleted during the epoch - no-op */
    return;
  }

  fsevent_probe(ev);

  if(ev->mask & IN_DELETE)
  {
    /* entity was deleted */
    RUNTIME_ASSERT(ev->kind == SWEEPER_EVENT_SELF);

    v = &n->vertex;
    ue = v->up_identity;

    /* deleted the root node ?? */
    RUNTIME_ASSERT(ue);

    graph_disintegrate(ue, invalidation);
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
      walker_descend(0, 0, cev->parent, path, 0, invalidation);
    }
    else
    {
      fsent_create(&n, VERTEX_TYPE_FSENT, VERTEX_OK, cev->name, cev->name_len);
      fsedge_connect(cev->parent, n, invalidation);
    }
  }
  else
  {
    /* entity was modified */
    fsent_exists_set(n);
    fsent_invalidate(n, invalidation);
  }
}

static void sweep()
{
  graph_invalidation_context invalidation = { };
  sweeper_event *ev;
  sweeper_child_event *ce;
  llist *cursor;

  graph_invalidation_begin(&invalidation);

  llist_foreach_safe(&event_queue, ev, lln, cursor) {
    /* subsequent events have not yet expired */
    if((event_era - ev->era) < 2) {
      break;
    }

    process_event(ev, &invalidation);

    llist_delete(ev, lln);
    if(ev->kind == SWEEPER_EVENT_SELF) {
      ev->era = 0;
    } else if(ev->kind == SWEEPER_EVENT_CHILD) {
      llist_append(&child_event_freelist, ev, lln);
      ce = containerof(ev, sweeper_child_event, sweep_event);
      rbtree_delete(&ce->parent->pending_child, ce, rbn);
    }
  }

  graph_invalidation_end(&invalidation);
}

static void sweeper_thread()
{
  sigset_t sigs;
  struct timespec deadline;
  sigset_t orig;
  int r;
  uint64_t period;
  rcu_thread rcu_self = { };

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

  period = g_args.sweeper_period_nsec;

  xclock_gettime(CLOCK_MONOTONIC, &deadline);
  timespec_add(&deadline, period);

  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    spinlock_acquire(&event_queue_lock);
    event_era++;

    if(!llist_empty(&event_queue))
    {
      sweep();
      if(llist_empty(&event_queue) && run_thread_autorun)
      {
        run_thread_launch(0, COMMAND_AUTORUN);
      }
    }

    spinlock_release(&event_queue_lock);

    /* sleep */
    xpthread_sigmask(SIG_SETMASK, &sigs, &orig);
    r = uxclock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, 0);
    xpthread_sigmask(SIG_SETMASK, &orig, 0);

    /* only advance the deadline if the sleep was not interrupted */
    if(r) {
      continue;
    }

    timespec_add(&deadline, period);
  }

  spinlock_release(&event_queue_lock);
  rcu_unregister(&rcu_self);
}

static void * sweeper_thread_jump(void * arg)
{
  tid = g_params.thread_sweeper = gettid();

  sweeper_thread();

  atomic_dec(&g_params.thread_count);
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

void sweeper_thread_enqueue(fsent *n, uint32_t mask, const char * restrict name, uint16_t namel)
{
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
        xmalloc(&ce, sizeof(*ce));
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

  spinlock_release(&event_queue_lock);
}

void sweeper_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, sweeper_thread_jump, 0);

  pthread_attr_destroy(&attr);
}

void sweeper_thread_cleanup()
{
  sweeper_child_event *ce;
  llist *cursor;

  llist_foreach_safe(&child_event_freelist, ce, sweep_event.lln, cursor) {
    free(ce);
  }
}
