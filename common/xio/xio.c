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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "xio.h"

int xopen(const char * path, int flags, int * const fd)
{
	if((*fd = open(path, flags)) == -1)
		return -1;

	return 0;
}

int xopen_mode(const char * path, int flags, mode_t mode, int * const fd)
{
	if((*fd = open(path, flags, mode)) == -1)
		return -1;

	return 0;
}

int xread(int fd, void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && (*bytes = read(fd, buf, count)) == -1)
		return -1;
	else if(read(fd, buf, count) == -1)
		return -1;

	return 0;
}

int xopendir(const char * name, DIR ** dd)
{
	if(((*dd) = opendir(name)) == 0)
	{
		return -1;
	}

	return 0;
}

int xreaddir_r(DIR * dirp, struct dirent * entry, struct dirent ** result)
{
	int r;
	if((r = readdir_r(dirp, entry, result)))
	{
		errno = r;
	}

	return 0;
}
