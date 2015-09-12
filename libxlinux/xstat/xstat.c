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

#define restrict __restrict

API xapi xstat(const char * path, struct stat * buf)
{
  enter;

#if 0
	if((r && ((*r) = stat(path, buf)) != 0) || (!r && stat(path, buf) != 0))
		fail(errno);
#endif
	if(stat(path, buf) != 0)
		fail(errno);
	
finally:
	XAPI_INFOF("path", "%s", path);
coda;
}

API xapi uxstat(const char * path, struct stat * buf, int * r)
{
  enter;

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

API xapi xlstat(const char * path, struct stat * buf, int * r)
{
  enter;

	if((r && ((*r) = lstat(path, buf)) != 0) || (!r && lstat(path, buf) != 0))
		fail(errno);
	
finally:
	XAPI_INFOF("path", "%s", path);
coda;
}

API xapi uxlstat(const char * path, struct stat * buf, int * r)
{
  enter;

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

API xapi xfstat(int fd, struct stat * buf)
{
  enter;

	if(fstat(fd, buf) != 0)
		fail(errno);
	
	finally : coda;
}

API xapi uxfstat(int fd, struct stat * buf)
{
  enter;

	if(fstat(fd, buf) != 0)
	{
		if(errno != ENOENT)
			fail(errno);

		memset(buf, 0, sizeof(*buf));
	}
	
finally:
	XAPI_INFOF("fd", "%d", fd);
coda;
}

API xapi xfutimens(int fd, const struct timespec times[2])
{
  enter;

	fatalize(errno, futimens, fd, times);

	finally : coda;
}

API xapi xutimensat(int dirfd, const char * const restrict pathname, const struct timespec times[2], int flags)
{
  enter;

	fatalize(errno, utimensat, dirfd, pathname, times, flags);

	finally : coda;
}

API xapi uxutimensat(int dirfd, const char * const restrict pathname, const struct timespec times[2], int flags, int * restrict r)
{
  enter;

	if((r && ((*r) = utimensat(dirfd, pathname, times, flags)) != 0) || (!r && utimensat(dirfd, pathname, times, flags) != 0))
	{
		if(errno != ENOENT)
			fail(errno);
	}

finally:
	XAPI_INFOF("path", "%s", pathname);
coda;
}

API xapi xmkdir(const char * pathname, mode_t mode)
{
  enter;

	fatalize(errno, mkdir, pathname, mode);

finally:
	XAPI_INFOS("path", pathname);
coda;
}

API xapi uxmkdir(const char * pathname, mode_t mode)
{
  enter;

	if(mkdir(pathname, mode) != 0 && errno != EEXIST)
		fail(errno);

finally:
	XAPI_INFOS("path", pathname);
coda;
}

API xapi xfchmod(int fd, mode_t mode)
{
  enter;

	fatalize(errno, fchmod, fd, mode);

finally:
	XAPI_INFOF("fd", "%d", fd);
coda;
}
