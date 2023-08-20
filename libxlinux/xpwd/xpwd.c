/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "xpwd/xpwd.h"

void API uxgetpwuid_r(uid_t uid, struct passwd * pwd, char * buf, size_t buflen, struct passwd ** result)
{
  if(getpwuid_r(uid, pwd, buf, buflen, result) == 0)
  {
    // possibly found, check *result
  }
  else if(errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM)
  {
    *result = 0;  // uid not found
  }
  else
  {
    RUNTIME_ABORT();
  }
}

struct passwd * API xgetpwuid(uid_t uid)
{
  struct passwd * pwd;

  errno = 0;
  pwd = getpwuid(uid);
  RUNTIME_ASSERT(pwd != 0);

  return pwd;
}
