/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef _SPINLOCK
#define _SPINLOCK

#include <sched.h>

#include "xlinux/xunistd.h"

#include "atomic.h"

static inline void spinlock_engage_as(int * v, int as)
{
  uint16_t x;

  if(*v != as)
  {
    for(x = 0; ; x++)
    {
      if(atomic_cmpxchg(v, 0, as) == 0)
      {
        break;
      }
      else if((x % 256) == 0)
      {
        sched_yield();
      }
    }
  }
}

static inline void spinlock_engage(int * v)
{
  spinlock_engage_as(v, gettid());
}

static inline void spinlock_release(int * v)
{
  *v = 0;
}

#endif
