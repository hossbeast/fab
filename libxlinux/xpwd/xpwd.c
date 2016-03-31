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

#include "internal.h"
#include "xpwd/xpwd.h"
#include "errtab/SYS.errtab.h"

API xapi uxgetpwuid_r(uid_t uid, struct passwd * pwd, char * buf, size_t buflen, struct passwd ** result)
{
  enter;

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
    fail(errno);
  }

finally :
  XAPI_INFOF("uid", "%zu", uid);
coda;
}

API xapi xgetpwuid(uid_t uid, struct passwd ** const pwd)
{
  enter;

  errno = 0;
  if(((*pwd) = getpwuid(uid)) == 0)
    fail(errno);

  finally : coda;
}
