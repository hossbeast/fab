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

#ifndef _ATOMIC_H
#define _ATOMIC_H

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

static inline bool atomic_cas_i32(int32_t *v, int32_t *old, int32_t *new)
{
  return __atomic_compare_exchange(v, old, new, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline int atomic_cas_u32(uint32_t *v, uint32_t *old, uint32_t *new)
{
  return __atomic_compare_exchange(v, old, new, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

#endif
