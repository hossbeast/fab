/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xmem.h"

int xmalloc(void* target, size_t size)
{
	void** t = ((void**)target);
	*t = calloc(size, 1);
	return *t ? 1 : 0;
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

			return 1;
		}

		return 0;
	}

	return 1;
}

void xfree(void* target)
{
	void** t = (void**)target;

	free(*t);
	*t = 0;
}
