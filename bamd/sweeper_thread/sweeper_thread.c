/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <sys/inotify.h>
#include <errno.h>
#include <sys/stat.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xpthread.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xtime.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"
#include "xlinux/xsignal.h"
#include "bacon/writer.h"
#include "bacon/value.h"
#include "bacon/location.h"
#include "bacon/builder.h"

#include "sweeper_thread.h"
#include "args.h"
#include "fsedge.h"
#include "fsent.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "goals.h"
#include "graph.h"
#include "disintegrate.h"
#include "run_thread.h"
#include "params.h"
#include "rcu.h"
#include "walker.h"
#include "zcat.h"
#include "probes.h"

#include "locks.h"
#include "atomics.h"
#include "threads.h"
#include "times.h"
#include "filesystem.h"
#include "inotify_mask.h"
#include "edges.h"
#include "vertices.h"
#include "config.h"
#include "timers.h"
#include "events/events.h"
#include "system.h"
#include "symlink.h"

/* duration of sleeps in the sweeper thread, which forms the maximum precision of the sweeper clock
 *  100ms */
#define NOTIFY_PERIOD_MINIMUM_NSEC MSEC_AS_NSEC(100)

/* effective sweep interval */
uint64_t sweeper_thread_interval = MSEC_AS_NSEC(500);
uint32_t sweeper_thread_clock;      // advances in units of the interval

static llist event_queue = LLIST_INITIALIZER(event_queue);
static llist child_event_freelist = LLIST_INITIALIZER(child_event_freelist);
static struct futexlock event_queue_lock;
static bool relaunch;

static void process_event(sweeper_event * restrict ev, graph_invalidation_context * restrict invalidation)
{
  char path[PATH_MAX];
  uint16_t path_len;
  moria_vertex *v;
  moria_edge *ue;
  fsent *n;
  struct vertices unlinking;
  struct vertices releasing;
  struct edges edges;
  sweeper_child_event *cev;
  size_t z;
  struct stat stb;

  sweeper_thread_probe(ev);

  if(ev->kind == SWEEPER_EVENT_SELF)
  {
    n = containerof(ev, fsent, pending);
    cev = 0;
  }
  else
  {
    RUNTIME_ASSERT(ev->kind == SWEEPER_EVENT_CHILD);
    cev = containerof(ev, typeof(*cev), sweep_event);
    n = cev->parent;
  }

  if(ev->mask & (IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_FROM))
  {
    if(ev->mask & (IN_CREATE | IN_MOVED_TO)) {
      /* entity created then subsequently deleted during the epoch - no-op */
      return;
    }

    if(ev->kind == SWEEPER_EVENT_SELF)
    {
      v = &n->vertex;
    }
    else
    {
      RUNTIME_ASSERT(ev->kind == SWEEPER_EVENT_CHILD);
      v = &cev->parent->vertex;
      if((v = moria_vertex_downw(v, cev->name, cev->name_len)) == 0) {
        /* delete event for an fsent which no longer exists */
        return;
      }
    }

    ue = v->up_identity;

    /* deleted the root node ?? */
    RUNTIME_ASSERT(ue);

    vertices_init(&unlinking);
    vertices_init(&releasing);
    edges_init(&edges);
    vertices_push(&unlinking, v);
    graph_disintegrate(&edges, &unlinking, &releasing, invalidation);
  }
  else if((ev->mask & (IN_CREATE | IN_MOVED_TO)) && ev->kind == SWEEPER_EVENT_CHILD)
  {
    /* absolute path to the child */
    z = fsent_absolute_path_cat(path, sizeof(path), cev->parent);
    z += zcatc(path + z, sizeof(path) - z, '/');
    z += zcatw(path + z, sizeof(path) - z, cev->name, cev->name_len);
    path[z] = 0;
    path_len = z;

    if(ev->mask & IN_ISDIR)
    {
      /* cev->parent is the parent of the newly created directory */
      fsent_create(&n, VERTEX_FILETYPE_DIR, VERTEX_OK, cev->name, cev->name_len);
      fsedge_connect(n, cev->parent, invalidation);
      vertex_alloc_probe(&n->vertex);

      walker_descend(n, path, 0, invalidation);
    }
    else
    {
      /* unfortunately the inotify event itself cannot distinguish symlinks */
      if(uxfstatats(INT32_MAX, AT_SYMLINK_NOFOLLOW, &stb, path) != 0) {
        /* fsent no longer exists */
        return;
      }

      if((stb.st_mode & S_IFMT) == S_IFLNK) {
        /* symlink was created */
        fsent_create(&n, VERTEX_SYMLINK, VERTEX_UNCREATED, cev->name, cev->name_len);
      } else {
        /* file was created */
        fsent_create(&n, VERTEX_FILE, VERTEX_UNCREATED, cev->name, cev->name_len);
      }

      fsedge_connect(n, cev->parent, invalidation);
      vertex_alloc_probe(&n->vertex);
      walker_poke(n, path, path_len, invalidation);

      if((stb.st_mode & S_IFMT) == S_IFLNK) {
        symlink_readlink_push(n); /* resolve the symlink */
      }

      fsent_unlink_candidate(n);
    }
  }
  else
  {
    /* entity was modified */
    vertex_exists_set(&n->vertex, true);
    vertex_invalidate(&n->vertex, invalidation);
    if(vertex_type_get(&n->vertex) == VERTEX_SYMLINK) {
      symlink_readlink_push(n);
    }
  }
}

static void event_free(sweeper_event *ev)
{
  sweeper_child_event *ce;

  /*
   * holds event_queue_lock
   */

  if(ev->kind == SWEEPER_EVENT_SELF) {
    ev->time = 0;
  } else if(ev->kind == SWEEPER_EVENT_CHILD) {
    llist_append(&child_event_freelist, &ev->lln);
    ce = containerof(ev, sweeper_child_event, sweep_event);
    rbtree_delete(&ce->parent->pending_child, &ce->rbn);
  }
}

static void sweep()
{
  graph_invalidation_context invalidation = { };
  sweeper_event *ev;

  graph_invalidation_begin(&invalidation);

  while((ev = llist_shift(&event_queue, typeof(*ev), lln))) {
    /* events past this point in the list are not yet old enough to process */
    if(ev->time > sweeper_thread_clock) {
      llist_unshift(&event_queue, &ev->lln);
      break;
    }

    process_event(ev, &invalidation);
    event_free(ev);
  }

  graph_invalidation_end(&invalidation);
}

static void timer_handler(int token, int overruns)
{
  sweeper_thread_clock++;
  sweeper_thread_clock += overruns;
}

static void sweeper_thread()
{
  sigset_t sigs;
  rcu_thread rcu_self = { };
  sweeper_event *ev;
  struct timespec interval;
  timer_t timerid;

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  sigaddset(&sigs, SIGUSR2);

  /* rcu-respecting sleep */
  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(125);

  timers_periodic_create(&timerid, tid, 0x1, timer_handler);

  /* to publish events, sweeper thread is an rcu reader
   * this imposes a maximum limit on the time it can be suspended */
  rcu_enter(&rcu_self);
  while(!g_params.shutdown)
  {
    /* reset the clock and discard any events which had been enqueued under the old clock */
    futexlock_acquire(&event_queue_lock);
    while((ev = llist_shift(&event_queue, typeof(*ev), lln))) {
      event_free(ev);
    }
    futexlock_release(&event_queue_lock);

    /* arm or re-arm the timer according to the new clock configuration */
    sweeper_thread_clock = 0;
    timers_periodic_put(timerid, sweeper_thread_interval);

    relaunch = false;
    while(!g_params.shutdown && !relaunch)
    {
      rcu_quiesce(&rcu_self);

      if(llist_empty(&event_queue)) {
        goto next;
      }

      futexlock_acquire(&event_queue_lock);

      if(llist_empty(&event_queue)) {
        futexlock_release(&event_queue_lock);
        goto next;
      }

      sweep();
      if(llist_empty(&event_queue) && run_thread_autorun) {
        /* optimistic launch ; missed events are fine and ignored */
        run_thread_launch(0, 0, COMMAND_AUTORUN);
      }

      futexlock_release(&event_queue_lock);

next:
      /* rcu-respecting sleep */
      xpthread_sigmask(SIG_UNBLOCK, &sigs, 0);
      uxclock_nanosleep(CLOCK_MONOTONIC, 0, &interval, 0);
      xpthread_sigmask(SIG_BLOCK, &sigs, 0);
    }
  }

  rcu_leave(&rcu_self);
}

static void *sweeper_thread_jump(void * arg)
{
  tid = g_params.thread_sweeper = gettid();
  errnop = &errno;

  sweeper_thread();

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

void sweeper_thread_enqueue(fsent *n, uint32_t mask, const char * restrict name, uint16_t namel)
{
  sweeper_event *e;
  sweeper_child_event *ce;
  rbnode *rbn;
  rbtree_search_context search_ctx;
  struct event_rbn_key key;

  futexlock_acquire(&event_queue_lock);

  if(name)
  {
    /* event for an fsent in the watched directory */
    RUNTIME_ASSERT((mask & (IN_DELETE_SELF | IN_MOVE_SELF)) == 0);

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
        llist_init(&ce->sweep_event.lln);
      }
      else
      {
        ce->sweep_event.mask = 0;
      }

      memcpy(ce->name, name, namel);
      ce->name_len = namel;
      ce->parent = n;

      rbtree_insert(&n->pending_child, &search_ctx, &ce->rbn);
    }
    e = &ce->sweep_event;
  }
  else
  {
    e = &n->pending;
    if(e->time == 0) {
      e->mask = 0;
    }
  }

  /* ignore the delete if the entity was subsequently re-created during the epoch */
  if((mask & (IN_CREATE | IN_MOVED_TO))) {
    e->mask &= ~(IN_DELETE | IN_MOVED_FROM);
  }

  e->mask |= mask;

  /* refresh timer */
  e->time = sweeper_thread_clock + 2;

  /* push to the end */
  llist_delete(&e->lln);
  llist_append(&event_queue, &e->lln);

  futexlock_release(&event_queue_lock);
}

void sweeper_thread_cancel(fsent * restrict fse)
{
  if(!llist_attached(&fse->pending.lln)) {
    return;
  }

  /* if running on the sweeper thread itself, the lock is already held */
  if(futexlock_try(&event_queue_lock)) {
    llist_delete(&fse->pending.lln);
    futexlock_release(&event_queue_lock);
  } else {
    llist_delete(&fse->pending.lln);
  }
}

void sweeper_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, sweeper_thread_jump, 0);

  pthread_attr_destroy(&attr);
}

void sweeper_thread_reconfigure(struct config * restrict cfg, bool dryrun)
{
  uint64_t period_nsec;
  events_context ectx;
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  const bacon_location *loc;

  period_nsec = cfg->notify.interval.u32 * NSEC_PER_MSEC;
  if(period_nsec == 0) {
    period_nsec = g_args.sweeper_period_nsec;
  }

  if(dryrun) {
    if(period_nsec < NOTIFY_PERIOD_MINIMUM_NSEC) {
      msg = system_error_event(&ectx);
      narrator_fixed_init(&fixed, msg->text, msg->size);
      bacon_writer_init(&writer);
      builder_writer = bacon_writer_open(&writer, &fixed.base);

      { MAPPING STRING("invalid")
        { SET
          { MAPPING STRING("interval")
            UINT(period_nsec)
          }
          { MAPPING STRING("location")
            loc = cfg->notify.interval.v->loc;
            LOADF("%s [%d,%d - %d,%d]", loc->fname, loc->f_lin + 1, loc->f_col + 1, loc->l_lin + 1, loc->l_col + 1);
          }
        }
      }

      bacon_writer_close(&writer);
      msg->code = EINVAL;
      msg->len = fixed.l;
      events_publish(&ectx);
    }

    return;
  }

  sweeper_thread_interval = period_nsec;
  relaunch = true;
  uxtgkill(g_params.pid, g_params.thread_sweeper, SIGUSR1);
}

void sweeper_thread_cleanup()
{
  sweeper_child_event *ce;
  llist *cursor;

  llist_foreach_safe(&child_event_freelist, ce, sweep_event.lln, cursor) {
    free(ce);
  }
}
