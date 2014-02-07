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

#include "xfcntl.h"

int API xopen(const char * path, int flags, int * const fd)
{
	if(fd && (*fd = open(path, flags)) == -1)
		sysfatality("open");

	else if(open(path, flags) == -1)
		sysfatality("open");

finally:
	XAPI_INFO("path", "%s", path);
coda;
}

int API gxopen(const char * path, int flags, int * const fd)
{
	if((*fd = open(path, flags)) == -1)
	{
		if(errno != ENOENT)
			sysfatality("open");

		*fd = -1;
	}

finally:
	XAPI_INFO("path", "%s", path);
coda;
}

int API xopen_mode(const char * path, int flags, mode_t mode, int * const fd)
{
	if(fd && (*fd = open(path, flags, mode)) == -1)
		sysfatality("open");

	else if(open(path, flags, mode) == -1)
		sysfatality("open");

finally:
	XAPI_INFO("path", "%s", path);
coda;
}

int API gxopen_mode(const char * path, int flags, mode_t mode, int * const fd)
{
	if((*fd = open(path, flags, mode)) == -1)
	{
		if(errno != ENOENT)
			sysfatality("open");

		*fd = -1;
	}

finally:
	XAPI_INFO("path", "%s", path);
coda;
}
