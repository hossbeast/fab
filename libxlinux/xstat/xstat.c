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

#include <string.h>
#include <errno.h>

#include "internal.h"

#include "xstat.h"

int API xstat(const char * path, struct stat * buf, int * r)
{
	if((r && ((*r) = stat(path, buf)) != 0) || (!r && stat(path, buf) != 0))
		fail(errno);
	
finally:
	XAPI_INFOF("path", "%s", path);
coda;
}

int API uxstat(const char * path, struct stat * buf, int * r)
{
	if((r && ((*r) = stat(path, buf)) != 0) || (!r && stat(path, buf) != 0))
	{
		if(errno != ENOENT && errno != ENOTDIR)
			fail(errno);

		memset(buf, 0, sizeof(*buf));
	}
	
finally:
	XAPI_INFOF("path", "%s", path);
coda;
}

int API xlstat(const char * path, struct stat * buf, int * r)
{
	if((r && ((*r) = lstat(path, buf)) != 0) || (!r && lstat(path, buf) != 0))
		fail(errno);
	
finally:
	XAPI_INFOF("path", "%s", path);
coda;
}

int API uxlstat(const char * path, struct stat * buf, int * r)
{
	if((r && ((*r) = lstat(path, buf)) != 0) || (!r && lstat(path, buf) != 0))
	{
		if(errno != ENOENT)
			fail(errno);

		memset(buf, 0, sizeof(*buf));
	}
	
finally:
	XAPI_INFOF("path", "%s", path);
coda;
}

int API xfstat(int fd, struct stat * buf)
{
	if(fstat(fd, buf) != 0)
		fail(errno);
	
	finally : coda;
}

int API uxfstat(int fd, struct stat * buf)
{
	if(fstat(fd, buf) != 0)
	{
		if(errno != ENOENT)
			fail(errno);

		memset(buf, 0, sizeof(*buf));
	}
	
finally:
	XAPI_INFOF("fd", "%s", fd);
coda;
}

int API xfutimens(int fd, const struct timespec times[2])
{
	fatalize(errno, futimens, fd, times);

	finally : coda;
}

int API xmkdir(const char * pathname, mode_t mode)
{
	fatalize(errno, mkdir, pathname, mode);

finally:
	XAPI_INFOS("path", pathname);
coda;
}

int API uxmkdir(const char * pathname, mode_t mode)
{
	if(mkdir(pathname, mode) != 0 && errno != EEXIST)
		fail(errno);

finally:
	XAPI_INFOS("path", pathname);
coda;
}
