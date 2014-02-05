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

#include <string.h>
#include <errno.h>

#include "internal.h"

#include "xstat.h"

int API xstat(const char * path, struct stat * buf)
{
	if(stat(path, buf) != 0)
		sysfatality("stat");
	
finally:
	XAPI_INFO("path", "%s", path);
coda;
}

int API gxstat(const char * path, struct stat * buf)
{
	if(stat(path, buf) != 0)
	{
		if(errno != ENOENT)
			sysfatality("stat");

		memset(buf, 0, sizeof(*buf));
	}
	
finally:
	XAPI_INFO("path", "%s", path);
coda;
}

int API xfstat(int fd, struct stat * buf)
{
	if(fstat(fd, buf) != 0)
		sysfatality("fstat");
	
finally:
	XAPI_INFO("fd", "%s", fd);
coda;
}

int API uxfstat(int fd, struct stat * buf)
{
	if(fstat(fd, buf) != 0)
	{
		if(errno != ENOENT)
			sysfatality("fstat");

		memset(buf, 0, sizeof(*buf));
	}
	
finally:
	XAPI_INFO("fd", "%s", fd);
coda;
}
