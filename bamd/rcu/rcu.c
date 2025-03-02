/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <inttypes.h>

#include "bacon/writer.h"
#include "bacon/builder.h"
#include "xlinux/xtime.h"
#include "rcu.h"
#include "rcu_stats.desc.h"
#include "stats.h"
#include "system.h"
#include "vertex.h"
#include "shadow.h"

#include "locks.h"
#include "times.h"
#include "rcu.h"
#include "zcat.h"

union stats_rcu rcu_stats;

static struct {
  /* one bit per reader thread */
  uint64_t active;

  /* auto-inc synchronization counter */
  uint32_t sync;

  /* indicates readers which have quiesced in the current sync */
  uint64_t waitset;

  /* synchronizer lock */
  struct futexlock lock;
} state;

#if CONFIG_DEBUG || CONFIG_DEVEL
__thread bool rcu_is_reader;
#endif

static void stats_post(union stats_rcu *stats)
{
  descriptor_field *field;
  const union stats_record *src;
  union stats_record *dst;
  int x;

  src = &stats->record;
  dst = &rcu_stats.record;

  for(x = 0; x < descriptor_stats_rcu.members_len; x++)
  {
    field = descriptor_stats_rcu.members[x];
    if(0
      || field == &descriptor_stats_rcu_sync_active_max
      || field == &descriptor_stats_rcu_sync_lat_ns_max
    ) {
      continue;
    }
    switch(field->size) {
      case 1:
        atomic_add(&dst->u8[field->offset / 1], src->u8[field->offset / 1]);
        break;
      case 2:
        atomic_add(&dst->u16[field->offset / 2], src->u16[field->offset / 2]);
        break;
      case 4:
        atomic_add(&dst->u32[field->offset / 4], src->u32[field->offset / 4]);
        break;
      case 8:
        atomic_add(&dst->u64[field->offset / 8], src->u64[field->offset / 8]);
        break;
      default:
        RTA;
    }
  }

  /* yes lost updates are possible here */
  if(stats->sync_active_max > rcu_stats.sync_active_max) {
    rcu_stats.sync_active_max = stats->sync_active_max;
  }
  if(stats->sync_lat_ns_max > rcu_stats.sync_lat_ns_max) {
    rcu_stats.sync_lat_ns_max = stats->sync_lat_ns_max;
  }

  memset(stats, 0, sizeof(*stats));
}

//
// public
//

void rcu_enter(rcu_thread * restrict self)
{
  uint64_t prev;
  uint64_t active;
  uint32_t bit;
  uint64_t mark;

  memset(self, 0, sizeof(*self));

  /* claim the lowest un-set bit */
  prev = atomic_load(&state.active);
  do {
    if(prev == UINT64_MAX) {
      /* more than 64 concurrent readers are not supported */
      RTA;
    }
    active = prev;
    bit = ctzll(~prev);
    mark = 1ULL << bit;
    active |= mark;
  } while(!atomic_cas(&state.active, &prev, active));

  self->mark = mark;

#if CONFIG_DEBUG || CONFIG_DEVEL
  rcu_is_reader = true;
#endif

  self->stats.enter++;
}

void rcu_leave(rcu_thread * restrict self)
{
  uint64_t prev;
  uint64_t active;

  /* return the claimed bit */
  prev = atomic_load(&state.active);
  do {
    RUNTIME_ASSERT(prev != 0);
    active = prev;
    active &= ~self->mark;
  } while(!atomic_cas(&state.active, &prev, active));

  syscall(SYS_futex, &state.waitset, FUTEX_WAKE, 1);

  self->stats.leave++;
  stats_post(&self->stats);

#if CONFIG_DEBUG || CONFIG_DEVEL
  rcu_is_reader = false;
#endif
}

void rcu_synchronize()
{
  uint64_t active;
  uint64_t waitset;
  uint64_t init_active = 0;
  uint64_t runtime_ns;
  struct timespec start, end;
  struct timespec interval;
  bool timedout;
  union stats_rcu stats;

  char buf[256];
  uint16_t zl;

#if CONFIG_DEBUG || CONFIG_DEVEL
  RUNTIME_ASSERT(!rcu_is_reader);
#endif

  /* there is a possibility of missed wakeup from the FUTEX_WAKE in rcu_leave
   * the FUTEX_WAIT timeout here exists to catch such events */
  interval.tv_nsec = 0;
  interval.tv_sec = 1;

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &start);

  /* serialize synchronize calls */
  futexlock_acquire(&state.lock);
  atomic_inc(&state.sync);

  active = atomic_load(&state.active);
  init_active = active;
  timedout = false;
  while(1) {
    waitset = atomic_load(&state.waitset);
    /* the sync ends when all active threads are in the waitset */
    if((active & waitset) == active) {
      break;
    }
    if(syscall(SYS_futex, &state.waitset, FUTEX_WAIT, waitset, &interval, 0, 0) != 0) {
      if(errno == ETIMEDOUT) {
        timedout = true;
      }
    }
    active = atomic_load(&state.active);
  }

  atomic_store(&state.waitset, 0);
  futexlock_release(&state.lock);

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &end);
  runtime_ns = timespec_delta(&start, &end);

  init_active = __builtin_popcount(init_active);

  memset(&stats, 0, sizeof(stats));
  if(timedout) {
    stats.sync_timeout++;
  } else {
    stats.sync_fast++;
  }

  if(init_active < 4) {
    stats.sync_active_0_3++;
  } else if(init_active < 8) {
    stats.sync_active_4_7++;
  } else if(init_active < 16) {
    stats.sync_active_8_15++;
  } else if(init_active < 32) {
    stats.sync_active_16_31++;
  } else if(init_active < 64) {
    stats.sync_active_32_63++;
  } else {
    stats.sync_active_64_n++;
  }
  if(init_active > stats.sync_active_max) {
    stats.sync_active_max = init_active;
  }

  if(runtime_ns < MSEC_AS_NSEC(100)) {
    stats.sync_lat_0_100ms++;
  } else if(runtime_ns < MSEC_AS_NSEC(250)) {
    stats.sync_lat_100_250ms++;
  } else if(runtime_ns < MSEC_AS_NSEC(500)) {
    stats.sync_lat_250_500ms++;
  } else if(runtime_ns < MSEC_AS_NSEC(750)) {
    stats.sync_lat_500_750ms++;
  } else if(runtime_ns < MSEC_AS_NSEC(1000)) {
    stats.sync_lat_750_1000ms++;
  } else if(runtime_ns < SEC_AS_NSEC(2)) {
    stats.sync_lat_1_2s++;
  } else {
    if(runtime_ns < SEC_AS_NSEC(4)) {
      stats.sync_lat_2_4s++;
    } else if(runtime_ns < SEC_AS_NSEC(8)) {
      stats.sync_lat_4_8s++;
    } else {
      stats.sync_lat_8s_n++;
    }
  }
  if(runtime_ns > stats.sync_lat_ns_max) {
    stats.sync_lat_ns_max = runtime_ns;
  }

  stats_post(&stats);

  if(runtime_ns >= SEC_AS_NSEC(2)) {
    /*
     * it is unexpected for rcu synchronize to take this long
     * and it causes odd hangs/stalls so notify the user
     */
    zl = timespec_ns_cat(buf, sizeof(buf), runtime_ns);
    buf[zl] = 0;
    printf("WARN : rcu synchronize runtime %s\n", buf);
#if CONFIG_DEVEL
    backtracef();
#endif
  }
}

void rcu_quiesce(rcu_thread * restrict self)
{
  uint64_t prev;
  uint64_t waitset;
  uint32_t sync;

  sync = atomic_load(&state.sync);
  if(self->sync == sync) {
    if(++self->stats.quiesce_nosync == 32) {
      stats_post(&self->stats);
    }
    return;
  }

  /* signal my bit in the waitset */
  prev = atomic_load(&state.waitset);
  do {
    waitset = prev;
    waitset |= self->mark;
  } while(!atomic_cas(&state.waitset, &prev, waitset));

  syscall(SYS_futex, &state.waitset, FUTEX_WAKE, 1);

  self->sync = sync;
  if(++self->stats.quiesce_sync == 32) {
    stats_post(&self->stats);
  }
}

void rcu_setup(void)
{
}

void rcu_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  rcu_stats.active = __builtin_popcount(state.active);

  stats_build(&rcu_stats.record, &descriptor_stats_rcu, builder_writer);
}

void rcu_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      vertex_mappings(v, builder_writer);
      { MAPPING
        STRING("stats")
        { SET
          rcu_sys_stats_build(v, false, builder_writer);
        }
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}
