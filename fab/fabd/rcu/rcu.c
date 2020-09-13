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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#include "locks.h"

#include "types.h"
#include "rcu.h"

static struct {
  uint16_t active;      // number of active threads
  uint16_t quiescent;   // number of threads having acknowledged the current synchronization
  uint16_t target;      // number of active threads at the start of the current synchronization

  uint16_t n;           // synchronization counter
  int32_t lock;
  int32_t futex;
  pid_t pid;
  pid_t tid;
} state;

#define TABLE                      \
  DEF(futex_acquire_uncontended)   \
  DEF(futex_acquire_contended)     \
  DEF(futex_release_wake)          \
  DEF(synchronize_nobody)          \
  DEF(synchronize_wait)            \
  DEF(quiesce_nograce)             \
  DEF(quiesce_notlast)             \
  DEF(quiesce_wake)                \

#define DEF(x) static uint32_t x;
TABLE

void rcu_log(void);
void rcu_log(void)
{
  dup2(601, 1);

#undef DEF
#define DEF(x) printf("%-25s %d\n", #x, x);
TABLE
}

void rcu_register(rcu_thread * restrict self)
{
  __atomic_load(&state.n, &self->n, __ATOMIC_SEQ_CST);    // self->n = state.n
  __atomic_add_fetch(&state.active, 1, __ATOMIC_SEQ_CST); // active++
}

static uint32_t quiesce(rcu_thread * restrict self, uint16_t deactivate)
{
  uint16_t quiescent;
  uint16_t target;
  uint16_t n;
  union quiesced Xn;

  n = __atomic_load_n(&state.n, __ATOMIC_SEQ_CST);

  if(self->n == n) {
    __atomic_sub_fetch(&state.active, deactivate, __ATOMIC_SEQ_CST);
    quiesce_nograce++;
    Xn.n = n;
    Xn.r = 1;
    return Xn.u32;
  }

  self->n = n;
  target = __atomic_load_n(&state.target, __ATOMIC_SEQ_CST);

  /* serialized gate */
  quiescent = __atomic_fetch_add(&state.quiescent, 1, __ATOMIC_SEQ_CST);

  __atomic_sub_fetch(&state.active, deactivate, __ATOMIC_SEQ_CST);

  // not the last one through the gate
  if(quiescent != (target - 1)) {
    quiesce_notlast++;
    Xn.n = n;
    Xn.r = 2;
    return Xn.u32;
  }

  __atomic_store_n(&state.futex, 0, __ATOMIC_SEQ_CST);
  syscall(SYS_futex, &state.futex, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  quiesce_wake++;
  Xn.n = n;
  Xn.r = 3;
  return Xn.u32;
}

uint32_t rcu_unregister(rcu_thread * restrict rcu)
{
  //uint16_t active;

  //active = state.active;
  //__atomic_sub_fetch(&state.active, 1, __ATOMIC_SEQ_CST);

  return quiesce(rcu, 1);
}

uint32_t rcu_quiesce(rcu_thread * restrict self)
{
  return quiesce(self, 0);
}

void rcu_synchronize(int32_t tid)
{
  uint16_t active;
  //uint16_t n;

  futex_acquire(&state.lock, tid);

  if((active = __atomic_load_n(&state.active, __ATOMIC_SEQ_CST)) == 0) {
    futex_release(&state.lock, tid);
//printf("synchronize nobody\n");
    synchronize_nobody++;
    return;
  }

  __atomic_store_n(&state.target, active, __ATOMIC_SEQ_CST);  // target = active
  __atomic_store_n(&state.futex, 1, __ATOMIC_SEQ_CST);        // futex = 1
  __atomic_store_n(&state.quiescent, 0, __ATOMIC_SEQ_CST);    // quiescent = 0
  __atomic_fetch_add(&state.n, 1, __ATOMIC_SEQ_CST);          // n++

//printf("synchronize -- waiting : n %hu\n", n);

  syscall(SYS_futex, &state.futex, FUTEX_WAIT, 1, 0, 0, 0);

  futex_release(&state.lock, tid);

//printf("synchronize -- waited : n %hu\n", n);

  synchronize_wait++;
}

#if 0
//{
//  int32_t one = 1;
//  int32_t zero = 0;
//
//  if(atomic_cas_i32(&g_rcu_waiters, &one, &zero)) {
//    syscall(SYS_futex, &g_rcu_waiters, FUTEX_WAKE, INT_MAX, 0, 0, 0);
//  }
//}

  // next unused bit
  for(x = 0; x < sizeof(state.set) / sizeof(*state.set); x++)
  {
    if(state.set[x] == UINT64_MAX) {
      continue;
    }

    id = ~state.set[x] & -~state.set[x];
    state.set[x] |= id;
    break;
  }

  // otherwise, max number of concurrent threads has been exceeded
  RUNTIME_ASSERT(id);

  // assign thread index, add to threadlist
  self->index = x;
  self->place = id;
  llist_append(rcus, self, lln);


//  while(1)
//  {
//    quiescent = state.quiescent;
//    if((__atomic_compare_exchange_n(&state.quiescent, &quiescent, quiescent + 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))) {
//      break;
//    }
//  }

static inline void rcu_futex_acquire(int32_t * futex)
{
  int x;

  for(x = 0; 1; x++)
  {
    if((__atomic_compare_exchange_n(futex, &zero, 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))) {
      break;
    }

    syscall(SYS_futex, futex, FUTEX_WAIT, 1, 0, 0, 0);
  }

  if(x == 0) {
    futex_acquire_uncontended++;
  } else {
    futex_acquire_contended++;
  }
}

static inline void rcu_futex_release(int32_t * futex)
{
  __atomic_store_n(futex, 0, __ATOMIC_SEQ_CST);
//  *futex = 0;
//  smp_wmb();
  syscall(SYS_futex, futex, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  futex_release_wake++;
}
#endif
