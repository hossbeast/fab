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

#include <sys/uio.h>

#include "internal.h"

xapi API xreadv(int fd, const struct iovec * iov, int iovcnt)
{
	xproxy(readv, fd, iov, iovcnt);
}

xapi API axreadv(int fd, const struct iovec * iov, int iovcnt)
{
	ssize_t expected = 0;
	ssize_t actual;
	if((actual = readv(fd, iov, iovcnt)) == -1)
		fail(errno);

	int x;
	for(x = 0; x < iovcnt; x++)
		expected += iov[x].iov_len;

	if(actual != expected)
		fail(XLINUX_LESS);

finally:
	if(XAPI_ERRCODE == XLINUX_LESS)
	{
		XAPI_INFOF("expected", "%zd", expected);
		XAPI_INFOF("actual", "%zd", actual);
	}
coda;
}

xapi API xwritev(int fd, const struct iovec * iov, int iovcnt)
{
	xproxy(writev, fd, iov, iovcnt);
}

xapi API axwritev(int fd, const struct iovec * iov, int iovcnt)
{
	ssize_t expected = 0;
	ssize_t actual;
	if((actual = writev(fd, iov, iovcnt)) == -1)
		fail(errno);

	int x;
	for(x = 0; x < iovcnt; x++)
		expected += iov[x].iov_len;

	if(actual != expected)
		fail(XLINUX_LESS);

finally:
	if(XAPI_ERRCODE == XLINUX_LESS)
	{
		XAPI_INFOF("expected", "%zd", expected);
		XAPI_INFOF("actual", "%zd", actual);
	}
coda;
}
