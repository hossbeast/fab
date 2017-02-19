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
#include "xgrp/xgrp.h"
#include "errtab/KERNEL.errtab.h"

API xapi uxgetgrgid_r(gid_t gid, struct group * grp, char * buf, size_t buflen, struct group ** result)
{
  enter;

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
    tfail(perrtab_KERNEL, errno);
  }

finally :
  xapi_infof("gid", "%zu", gid);
coda;
}

API xapi xgetgrgid(gid_t gid, struct group ** const grp)
{
  enter;

  errno = 0;
  if(((*grp) = getgrgid(gid)) == 0)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}
