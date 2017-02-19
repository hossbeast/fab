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

#ifndef _ATOMIC
#define _ATOMIC

/* intel syntax */

static inline void atomic_inc(int * v)
{
  asm("lock incl %0"
      : "+m" (*v));
}

static inline void atomic_dec(int * v)
{
  asm("lock decl %0"
      : "+m" (*v));
}

static inline int atomic_cmpxchg(int * v, int old, int new)
{
  int ret;
  asm volatile("lock cmpxchgl %2, %1"
               : "=a" (ret), "+m" (*v)
               : "r" (new), "0" (old)
               : "memory");
  return ret;
}

#endif
