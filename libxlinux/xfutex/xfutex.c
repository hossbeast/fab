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

#include <errno.h>

#include "xapi.h"
#include "types.h"

#include "xfutex.h"
#include "KERNEL.errtab.h"

xapi API uxfutex(
    int * restrict err
  , int32_t * restrict futex_wordp
  , int futex_op
  , int32_t val
  , const struct timespec * restrict timeout
  , int32_t * restrict uaddr2
  , int32_t val3
)
{
  enter;

  int r;

  if((r = syscall(SYS_futex, futex_wordp, futex_op, val, timeout, uaddr2, val3)) == -1)
  {
    if(errno != EAGAIN && errno != ETIMEDOUT && errno != EINTR) {
      tfail(perrtab_KERNEL, errno);
    }
    *err = errno;
  }
  else
  {
    *err = 0;
  }

  finally : coda;
}
