/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
  int32_t zero;

  RUNTIME_ASSERT(tid);

  /* blocks until the futex can transition from zero -> tid */
  while(1)
  {
    zero = 0;
    if((__atomic_compare_exchange_n(&lock->i32, &zero, tid, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))) {
      break;
    }

    zero = 0;
    syscall(SYS_futex, &lock->i32, FUTEX_WAIT, zero, 0, 0, 0);
  }
  smp_mb();

  return lock;
}

static inline void futexlock_release(struct futexlock * restrict lock)
{
  RUNTIME_ASSERT(tid);

#if DEBUG || DEVEL
  int32_t old;
  old = __atomic_exchange_n(&lock->i32, 0, __ATOMIC_SEQ_CST);
  RUNTIME_ASSERT(old == tid);
#else
  __atomic_store_n(&lock->i32, 0, __ATOMIC_SEQ_CST);
#endif

  syscall(SYS_futex, &lock->i32, FUTEX_WAKE, 1, 0, 0, 0);
}
