/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xgrp.h"

void API uxgetgrgid_r(gid_t gid, struct group * grp, char * buf, size_t buflen, struct group ** result)
{
  if(getgrgid_r(gid, grp, buf, buflen, result) == 0)
  {
    // possibly found, check *result
  }
  else if(errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM)
  {
    *result = 0;  // gid not found
  }
  else
  {
    RUNTIME_ABORT();
  }
}

struct group * API xgetgrgid(gid_t gid)
{
  struct group * grp;

  errno = 0;
  grp = getgrgid(gid);
  RUNTIME_ASSERT(grp != 0);

  return grp;
}
