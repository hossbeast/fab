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
#include "atomics.h"

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
  if(atomic_cas(&lock->i32, &zero, tid)) {
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
  atomic_cas(&lock->i32, &ltid, 0);
}

/* releases the lock without regard to the current state of the lock */
static inline void trylock_reset(struct trylock * restrict lock)
{
  atomic_store(&lock->i32, 0);
}

struct futexlock {
  int32_t i32;
};

static inline void futexlock_acquire(struct futexlock * restrict lock)
{
  int32_t prev;

  RUNTIME_ASSERT(tid);

  /* only overwrites the lock if taken by this thread */
  prev = 0;
  while(!atomic_cas(&lock->i32, &prev, tid)) {
    syscall(SYS_futex, &lock->i32, FUTEX_WAIT, prev, 0, 0, 0);
    prev = 0;
  }
}

static inline void futexlock_release(struct futexlock * restrict lock)
{
  atomic_store(&lock->i32, 0);

  /* wake one lock waiter */
  syscall(SYS_futex, &lock->i32, FUTEX_WAKE, 1);
}
