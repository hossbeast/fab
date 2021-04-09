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

#ifndef FABD_RCU_H
#define FABD_RCU_H

/*
Simple synchronization-counter based read-copy-update. In this program, rcu is used only to mediate
access to the list of active clients.

Readers are threads which traverse the active clients list (typically by calling events_publish)
 * the monitor thread
 * the beholder thread
 * the build thread
 * the bootstrap thread
 * the sweeper thread
 * handler threads

Updaters are threads which call rcu_synchronize
 * handler threads

A thread may not be both a reader and an updater at the same time.
*/

#include "types.h"
#include "barriers.h"
#include "macros.h"

typedef struct rcu_thread {
  uint16_t n;          // synchronize counter
} rcu_thread;

/*
 * Make this thread an rcu reader. This is a promise to call rcu_quiesce soon and regularly. This
 * thread is then permitted to read rcu-synchronized objects with rcu_read.
 */
void rcu_register(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * Unregister an rcu reader thread, with an implied rcu_quiesce.
 */
void rcu_unregister(rcu_thread * restrict rcu)
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

static inline bool rcu_registered_check()
{
  extern __thread bool __attribute__((unused)) rcu_is_registered;
  RUNTIME_ASSERT(rcu_is_registered);

  return true;
}

#endif
