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
#include <string.h>

#include "xapi.h"

#include "xmem.h"

int xmalloc(void* target, size_t size)
{
	if(((*(void**)target) = calloc(size, 1)) == 0)
	{
		sysfatality("calloc");
	}
	
finally :
	XAPI_INFO("size", "%zu", size);
coda;
}

int xrealloc(void* target, size_t es, size_t ec, size_t oec)
{
	void** t = ((void**)target);
	*t = realloc(*t, es * ec);

	if(es * ec)
	{
		if(*t)
		{
			if(((ssize_t)ec - (ssize_t)oec) > 0)
				memset(((char*)*t) + (oec * es), 0, ((ssize_t)ec - (ssize_t)oec) * es);
		}
		else
		{
			sysfatality("realloc");
		}
	}

finally :
	XAPI_INFO("size", "%zu", es * ec);
coda;
}


void xfree(void* target)
{
	void** t = (void**)target;

	free(*t);
	*t = 0;
}
