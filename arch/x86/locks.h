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

#ifndef _LOCKS
#define _LOCKS

#include <stdint.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>

#include "barriers.h"
#include "macros.h"
#include "threads.h"

static inline bool trylock_acquire(int32_t *lock)
{
  int32_t zero = 0;

  RUNTIME_ASSERT(g_tid);

  return __atomic_compare_exchange_n(lock, &zero, g_tid, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline void trylock_release(int32_t *lock)
{
  RUNTIME_ASSERT(g_tid);

  __atomic_compare_exchange_n(lock, &g_tid, 0, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline void spinlock_acquire(int32_t *lock)
{
  int32_t zero;

  RUNTIME_ASSERT(g_tid);

  while(1)
  {
    zero = 0;
    if((__atomic_compare_exchange_n(lock, &zero, g_tid, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)))
    {
      break;
    }
    asm("PAUSE");
  }

  smp_mb();
}

static inline void spinlock_release(int32_t * lock)
{
  smp_mb();
  __atomic_compare_exchange_n(lock, &g_tid, 0, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline void futex_acquire(int32_t * futex)
{
  int32_t zero;

  RUNTIME_ASSERT(g_tid);

  /* blocks until the futex can transition from zero -> g_tid */
  while(1)
  {
    zero = 0;
    if((__atomic_compare_exchange_n(futex, &zero, g_tid, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))) {
      break;
    }

    syscall(SYS_futex, futex, FUTEX_WAIT, g_tid, 0, 0, 0);
  }
  smp_mb();
}

static inline void futex_release(int32_t * futex)
{
  RUNTIME_ASSERT(g_tid);

#if DEBUG || DEVEL
  int32_t old;
  old = __atomic_exchange_n(futex, 0, __ATOMIC_SEQ_CST);
  RUNTIME_ASSERT(old == g_tid);
#else
  __atomic_store_n(futex, 0, __ATOMIC_SEQ_CST);
#endif

  syscall(SYS_futex, futex, FUTEX_WAKE, 1, 0, 0, 0);
}

#endif
