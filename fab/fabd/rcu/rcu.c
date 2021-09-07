/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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
#include <sys/syscall.h>
#include <linux/futex.h>

#include "rcu.h"
#include "stats.h"

#include "locks.h"

static struct {
  uint16_t active;      // number of active threads
  uint16_t target;      // number of active threads when the current synchronization started
  uint16_t quiescent;   // number of threads having acknowledged the current synchronization

  uint16_t n;           // synchronization counter
  struct futexlock lock;
  int32_t futex;
  pid_t pid;
  pid_t tid;
} state;

#if DEBUG || DEVEL
__thread bool rcu_is_registered;
#endif

static void quiesce(rcu_thread * restrict self, uint16_t deactivate)
{
  uint16_t quiescent;
  uint16_t target;
  uint16_t n;

  n = __atomic_load_n(&state.n, __ATOMIC_SEQ_CST);

  if(self->n == n) {
    __atomic_sub_fetch(&state.active, deactivate, __ATOMIC_SEQ_CST);
    STATS_INC(g_stats.rcu_quiesce_nograce);
    return;
  }

  self->n = n;
  target = __atomic_load_n(&state.target, __ATOMIC_SEQ_CST);

  /* serialized gate */
  quiescent = __atomic_fetch_add(&state.quiescent, 1, __ATOMIC_SEQ_CST);

  __atomic_sub_fetch(&state.active, deactivate, __ATOMIC_SEQ_CST);

  // not the last one through the gate
  if(quiescent != (target - 1)) {
    STATS_INC(g_stats.rcu_quiesce_notlast);
    return;
  }

  /* wake waiters if any */
  __atomic_store_n(&state.futex, 0, __ATOMIC_SEQ_CST);
  syscall(SYS_futex, &state.futex, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  STATS_INC(g_stats.rcu_quiesce_wake);
}

//
// public
//

void rcu_unregister(rcu_thread * restrict rcu)
{
  quiesce(rcu, 1);

#if DEBUG || DEVEL
  rcu_is_registered = false;
#endif
}

void rcu_quiesce(rcu_thread * restrict self)
{
  quiesce(self, 0);
}

void rcu_register(rcu_thread * restrict self)
{
  __atomic_load(&state.n, &self->n, __ATOMIC_SEQ_CST);    // self->n = state.n
  __atomic_add_fetch(&state.active, 1, __ATOMIC_SEQ_CST); // active++

#if DEBUG || DEVEL
  rcu_is_registered = true;
#endif
}

void rcu_synchronize()
{
  uint16_t active;

  futexlock_acquire(&state.lock);

  if((active = __atomic_load_n(&state.active, __ATOMIC_SEQ_CST)) == 0) {
    futexlock_release(&state.lock);
    STATS_INC(g_stats.rcu_synchronize_nobody);
    return;
  }

  __atomic_store_n(&state.target, active, __ATOMIC_SEQ_CST);  // target = active
  __atomic_store_n(&state.futex, 1, __ATOMIC_SEQ_CST);        // futex = 1
  __atomic_store_n(&state.quiescent, 0, __ATOMIC_SEQ_CST);    // quiescent = 0
  __atomic_fetch_add(&state.n, 1, __ATOMIC_SEQ_CST);          // n++

  syscall(SYS_futex, &state.futex, FUTEX_WAIT, INT32_MAX, 0, 0, 0);

  futexlock_release(&state.lock);
  STATS_INC(g_stats.rcu_synchronize_wait);
}
