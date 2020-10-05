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

#ifndef _RCU_H
#define _RCU_H

#include <signal.h>

#include "types.h"

typedef struct rcu_thread {
  uint16_t n;          // synchronize counter
} rcu_thread;

/*
 * register a thread for rcu
 */
void rcu_register(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * unregister a thread for rcu
 */
void rcu_unregister(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * release rcu references on this thread
 */
void rcu_quiesce(rcu_thread * restrict rcu)
  __attribute__((nonnull));

/*
 * block until rcu references held by any registered thread are released
 */
void rcu_synchronize(void);

/*
 * acquire a reference to an object through an rcu-protected pointer
 */
#define rcu_read(x) ACCESS_ONCE(x)

/*
 * publish the state of an object to an rcu-protected pointer
 */
#define rcu_update(x, y) do {   \
  smp_wmb();                    \
  ACCESS_ONCE(x) = (y);         \
} while(0)

#endif
