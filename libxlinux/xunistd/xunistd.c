/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#include "xunistd.h"

int API xread(int fd, void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && (*bytes = read(fd, buf, count)) == -1)
		sysfatality("read");

	else if(read(fd, buf, count) == -1)
		sysfatality("read");

	finally : coda;
}

int API xwrite(int fd, const void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && (*bytes = write(fd, buf, count)) == -1)
		sysfatality("write");

	else if(write(fd, buf, count) == -1)
		sysfatality("write");

	finally : coda;
}

int API xgetcwd(char * buf, size_t size, char ** res)
{
	if(res && (((*res) = getcwd(buf, size)) == 0))
		sysfatality("getcwd");

	else if(getcwd(buf, size) == 0)
		sysfatality("getcwd");

	finally : coda;
}

int API xlseek(int fd, off_t offset, int whence, off_t * res)
{
	if(res && ((*res) = lseek(fd, offset, whence)) == (off_t)-1)
		sysfatality("lseek");

	else if(lseek(fd, offset, whence) == (off_t)-1)
		sysfatality("lseek");

	finally : coda;
}

int API xclose(int fd)
{
	sysfatalize(close, fd);

	finally : coda;
}

int API ixclose(int * fd)
{
	if(*fd != -1)
	{
		sysfatalize(close, *fd);
		*fd = -1;
	}

	finally : coda;
}
