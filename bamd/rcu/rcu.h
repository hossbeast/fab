/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
synchronization-counter based read-copy-update. In this program, rcu is used only to mediate
access to the lists of active channels, and active plugins.

Readers are threads which traverse the lists (typically by calling events_publish)
 * the monitor thread
 * the sweeper thread
 * the run thread
 * a channel thread
 * a plugin thread

Updaters are threads which call rcu_synchronize to reclaim objects from the lists
 * a channel thread
 * a plugin thread

A thread may not be both a reader and an updater at the same time.
*/

#include "types.h"
#include "stats.h"
#include "descriptor.h"
#include "barriers.h"

struct fsent;
struct bacon_writer;
struct moria_vertex;

typedef union stats_rcu {
  struct {
      uint32_t sync_fast;           /* synchronize called */
      uint32_t sync_timeout;        /* synchronize called, and experienced a timeout */
      uint32_t quiesce_nosync;      /* quiesce called and no sync was underway */
      uint32_t quiesce_sync;        /* quiesce called while sync underway */
      uint32_t enter;               /* enter called */
      uint32_t leave;               /* leave called */
      uint32_t active;              /* active readers */

      uint32_t sync_active_0_3;     /* counts sync events by active readers */
      uint32_t sync_active_4_7;
      uint32_t sync_active_8_15;
      uint32_t sync_active_16_31;
      uint32_t sync_active_32_63;
      uint32_t sync_active_64_n;
      uint32_t sync_active_max;

      uint32_t sync_lat_0_100ms;    /* counts sync events by runtime */
      uint32_t sync_lat_100_250ms;
      uint32_t sync_lat_250_500ms;
      uint32_t sync_lat_500_750ms;
      uint32_t sync_lat_750_1000ms;
      uint32_t sync_lat_1_2s;
      uint32_t sync_lat_2_4s;
      uint32_t sync_lat_4_8s;
      uint32_t sync_lat_8s_n;
      uint64_t sync_lat_ns_max;
  };

  stats_record record;
} stats_rcu;

typedef struct rcu_thread {
  /* uniquely identifies this thread */
  uint64_t mark;

  /* last sync seen */
  uint32_t sync;

  union stats_rcu stats;
} rcu_thread;

extern union stats_rcu rcu_stats;
extern struct subsys sys_rcu;
extern descriptor_type descriptor_stats_rcu;

void rcu_sys_stats_build(const struct moria_vertex * restrict, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void rcu_sys_describe(const struct moria_vertex * restrict, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void rcu_setup(void);

/*
 * Make this thread an rcu reader. This is a promise to call rcu_quiesce soon and regularly. This
 * thread is then permitted to read rcu-synchronized objects with rcu_read.
 */
void rcu_enter(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * Unregister an rcu reader thread, with an implied rcu_quiesce.
 */
void rcu_leave(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * Called by an rcu reader to release rcu references on this thread.
 */
void rcu_quiesce(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * Called by an rcu updater to block until rcu references on all threads are released.
 */
void rcu_synchronize(void);

/* force volatile semantics for this particular access */
#define ACCESS_ONCE(x) (*((volatile typeof(x) *) &(x)))

/* acquire a reference to an rcu-synchronized object */
#define rcu_read(x) ({ rcu_registered_check(), ACCESS_ONCE(x); })

/* publish the state of an rcu-synchronized object */
#define rcu_update(x, y) do {                         \
  /* guarantee writes to the object are committed */  \
  smp_wmb();                                          \
  ACCESS_ONCE(x) = (y);                               \
} while(0)

#if CONFIG_DEBUG || CONFIG_DEVEL
extern __thread bool rcu_is_reader;
#endif

static inline bool rcu_registered_check()
{
#if CONFIG_DEBUG || CONFIG_DEVEL
  RUNTIME_ASSERT(rcu_is_reader);
#endif

  return true;
}
