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
#include <sys/uio.h>

#include "internal.h"
#include "xuio/xuio.h"
#include "errtab/KERNEL.errtab.h"
#include "errtab/XLINUX.errtab.h"

API xapi xreadv(int fd, const struct iovec * iov, int iovcnt)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, readv, fd, iov, iovcnt);

  finally : coda;
}

API xapi axreadv(int fd, const struct iovec * iov, int iovcnt)
{
  enter;

  ssize_t expected = 0;
  ssize_t actual;
  if((actual = readv(fd, iov, iovcnt)) == -1)
    tfail(perrtab_KERNEL, errno);

  int x;
  for(x = 0; x < iovcnt; x++)
    expected += iov[x].iov_len;

  if(actual != expected)
  {
    xapi_fail_intent();
    xapi_info_addf("expected", "%zd", expected);
    xapi_info_addf("actual", "%zd", actual);
    fail(XLINUX_LESS);
  }

  finally : coda;
}

API xapi xwritev(int fd, const struct iovec * iov, int iovcnt)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, writev, fd, iov, iovcnt);

  finally :  coda;
}

API xapi axwritev(int fd, const struct iovec * iov, int iovcnt)
{
  enter;

  ssize_t expected = 0;
  ssize_t actual;
  if((actual = writev(fd, iov, iovcnt)) == -1)
    tfail(perrtab_KERNEL, errno);

  int x;
  for(x = 0; x < iovcnt; x++)
    expected += iov[x].iov_len;

  if(actual != expected)
  {
    xapi_fail_intent();
    xapi_info_addf("expected", "%zd", expected);
    xapi_info_addf("actual", "%zd", actual);
    fail(XLINUX_LESS);
  }

  finally : coda;
}
