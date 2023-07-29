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

#ifndef FABD_LOCKS
#define FABD_LOCKS

#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>

#include "types.h"

#include "barriers.h"
#include "macros.h"
#include "threads.h"

struct trylock {
  int32_t i32;
};

/* initializer for a lock in the held state */
#define TRYLOCK_INIT_HELD { .i32 = INT32_MAX }

static inline bool trylock_acquire(struct trylock * restrict lock)
{
  int32_t zero = 0;

  RUNTIME_ASSERT(tid);

  /* the second argument to cas_n is read/write */
  if(__atomic_compare_exchange_n(&lock->i32, &zero, tid, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
    return true;
  }

  return false;
}

/* If the lock is held by the caller, releases the lock, otherwise does nothing */
static inline void trylock_release(struct trylock * restrict lock)
{
  int32_t ltid;

  RUNTIME_ASSERT(tid);

  ltid = tid;
  __atomic_compare_exchange_n(&lock->i32, &ltid, 0, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

/* releases the lock without regard to the current state of the lock */
static inline void trylock_reset(struct trylock * restrict lock)
{
  __atomic_store_n(&lock->i32, 0, __ATOMIC_SEQ_CST);
}

struct spinlock {
  int32_t i32;
};

static inline void spinlock_acquire(struct spinlock * restrict lock)
{
  int32_t zero;

  RUNTIME_ASSERT(tid);

  while(1) {
    zero = 0;
    if((__atomic_compare_exchange_n(&lock->i32, &zero, tid, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))) {
      break;
    }
    asm("PAUSE");
  }

  smp_mb();
}

static inline void spinlock_release(struct spinlock * restrict lock)
{
  int32_t ltid;

  RUNTIME_ASSERT(tid);

  ltid = tid;
  __atomic_compare_exchange_n(&lock->i32, &ltid, 0, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

struct futexlock {
  int32_t i32;
};

static inline struct futexlock *futexlock_acquire(struct futexlock * restrict lock)
{
  int zero;
  int prev;

  /* uncontended case */
  zero = 0;
  if((__atomic_compare_exchange_n(&lock->i32, &zero, 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))) {
    return lock;
  }

  while(1)
  {
    prev = __atomic_exchange_n(&lock->i32, 2, __ATOMIC_SEQ_CST);
    if(prev == 0) {
      break;
    }

    syscall(SYS_futex, &lock->i32, FUTEX_WAIT, 2, 0, 0, 0);
  }

  return lock;
}

static inline void futexlock_release(struct futexlock * restrict lock)
{
  int32_t prev;

  prev = __atomic_fetch_sub(&lock->i32, 1, __ATOMIC_SEQ_CST);

  if(prev == 1) {
    return;
  }

  __atomic_store_n(&lock->i32, 0, __ATOMIC_SEQ_CST);

  /* wake one lock waiter */
  syscall(SYS_futex, &lock->i32, FUTEX_WAKE, 1, 0, 0, 0);
}

#endif
