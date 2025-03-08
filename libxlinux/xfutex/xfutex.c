/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xfutex.h"

int uxfutex(
    int32_t * restrict futex_wordp
  , int futex_op
  , int32_t val
  , const struct timespec * restrict timeout
  , int32_t * restrict uaddr2
  , int32_t val3
)
{
  int r;

  if((r = syscall(SYS_futex, futex_wordp, futex_op, val, timeout, uaddr2, val3)) == -1)
  {
    r = errno;
    RUNTIME_ASSERT(r == EAGAIN || r == ETIMEDOUT || r == EINTR);
  }
  else
  {
    r = 0;
  }

  return r;
}
