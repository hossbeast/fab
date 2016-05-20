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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "memblk.def.h"

#define restrict __restrict

void memblk_freeze(struct memblk * const restrict mb, void * restrict _p)
{
	void ** p = _p;

	if(*p)
	{
		int x;
		for(x = 0; x < mb->blocksl; x++)
		{
			ptrdiff_t delta = (char*)*p - mb->blocks[x].s;
			if(delta >= 0 && delta < mb->blocks[x].l)
			{
#if 0
printf("%p -> %zu\n", *p, (size_t)(mb->blocks[x].o + delta));
#endif
				(*p) = (void*)(intptr_t)(mb->blocks[x].o + delta);
				return;
			}
		}

		// does not point into the memblk
	}
}

void memblk_unfreeze(struct memblk * const restrict mb, void * restrict _p)
{
	void ** p = _p;

	if(*p)
	{
		int x;
		for(x = 0; x < mb->blocksl; x++)
		{
			size_t delta = (intptr_t)(char*)*p - mb->blocks[x].o;

			if(delta >= 0 && delta < mb->blocks[x].l)
			{
#if 0
printf("%zu -> %p\n", (size_t)*p, mb->blocks[x].s + delta);
#endif
				*p = mb->blocks[x].s + delta;
				return;
			}
		}

		// does not point into the memblk
	}
}

void memblk_thaw(char * const restrict mb, void * restrict _p)
{
	void ** p = _p;

	if(*p)
	{
#if 0
printf("%p -> %zu\n", *p, (size_t)(intptr_t)(mb + (size_t)(intptr_t)*p));
#endif
		*p = mb + (size_t)(intptr_t)*p;
	}
}
