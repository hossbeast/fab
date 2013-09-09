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

#include <stdlib.h>
#include <string.h>

#include "listwise/internal.h"

#include "xmem.h"
#include "liblistwise_control.h"
#include "macros.h"

#define SAME  1
#define LEFT  2
#define RIGHT 3
#define INTER 4

int API lstack_window_add(lstack* const restrict ls, int x, int y, int off, int len)
{
	int type = LEFT;

	int i;
	for(i = 0; i < ls->s[x].w[y].l; i++)
	{
		if(off == ls->s[x].w[y].s[i].o && len == ls->s[x].w[y].s[i].l)
		{
			type = SAME; // same segment
			break;
		}
		else if((off + len) < ls->s[x].w[y].s[i].o)
		{
			type = LEFT; // disjoint on the left
			break;
		}
		else if(off > (ls->s[x].w[y].s[i].o + ls->s[x].w[y].s[i].l))
		{
			type = RIGHT; // disjoint on the right
		}
		else
		{
			type = INTER; // overlapping

			int noff = MIN(ls->s[x].w[y].s[i].o, off);
			int nlen = MAX(ls->s[x].w[y].s[i].o + ls->s[x].w[y].s[i].l, off + len) - noff;

			ls->s[x].w[y].s[i].o = noff;
			ls->s[x].w[y].s[i].l = nlen;

			break;
		}
	}

	if(type == LEFT || type == RIGHT || type == INTER)
	{
		if(type == LEFT || type == RIGHT)
		{
			// reallocate if necessary
			if(ls->s[x].w[y].a == ls->s[x].w[y].l)
			{
				int ns = ls->s[x].w[y].a ?: listwise_allocation_seed;
				ns = ns * 2 + ns / 2;

				fatal(xrealloc, &ls->s[x].w[y].s, sizeof(*ls->s[0].w[0].s), ns, ls->s[x].w[y].a);
				ls->s[x].w[y].a = ns;
			}

			// move down
			memmove(
				  &ls->s[x].w[y].s[i + 1]
				, &ls->s[x].w[y].s[i]
				, (ls->s[x].w[y].a - i - 1) * sizeof(ls->s[0].w[0].s[0])
			);

			ls->s[x].w[y].s[i].o = off;
			ls->s[x].w[y].s[i].l = len;
			ls->s[x].w[y].l++;
		}

		ls->s[x].w[y].y = 1;	// window : set
		ls->s[x].t[y].y = 0;	// temp : dirty
	}

	finally : coda;
}

int API lstack_window_clear(lstack* const restrict ls, int x, int y)
{
	ls->s[x].w[y].y = 0;
	ls->s[x].t[y].y = 0;

	return 0;
}

int API lstack_window_set(lstack* const restrict ls, int x, int y, int off, int len)
{
	ls->s[x].w[y].l = 0;

	return lstack_window_add(ls, x, y, off, len);
}
