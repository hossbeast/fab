/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include "internal.h"

#include "xlinux.h"
#include "macros.h"

#define restrict __restrict

#define SAME  1
#define LEFT  2
#define RIGHT 3
#define INTER 4

int API lstack_window_stage(lwx * const restrict lx, int y, int off, int len)
{
	if(lx->win.s[y].staged == 0)
	{
		lx->win.s[y].staged = &lx->win.s[y].storage[0];
		lx->win.s[y].staged_storage_index = 0;

		if(lx->win.s[y].staged == lx->win.s[y].active)
		{
			lx->win.s[y].staged = &lx->win.s[y].storage[1];
			lx->win.s[y].staged_storage_index = 1;
		}
		else if(lx->win.s[y].active == 0 && lx->win.s[y].staged->mark)
		{
			lx->win.s[y].staged = &lx->win.s[y].storage[1];
			lx->win.s[y].staged_storage_index = 1;
		}

		lx->win.s[y].staged->mark = 0;
		lx->win.s[y].staged->l = 0;
	}
	
	if(lx->win.s[y].staged->lease != lx->win.staged_era)
		lx->win.s[y].staged->l = 0;

	lx->win.s[y].staged->lease = lx->win.staged_era;

	int type = LEFT;

	int i;
	for(i = 0; i < lx->win.s[y].staged->l; i++)
	{
		if(off == lx->win.s[y].staged->s[i].o && len == lx->win.s[y].staged->s[i].l)
		{
			type = SAME; // same segment
			break;
		}
		else if((off + len) < lx->win.s[y].staged->s[i].o)
		{
			type = LEFT; // disjoint on the left
			break;
		}
		else if(off > (lx->win.s[y].staged->s[i].o + lx->win.s[y].staged->s[i].l))
		{
			type = RIGHT; // disjoint on the right
		}
		else
		{
			type = INTER; // overlapping

			int noff = MIN(lx->win.s[y].staged->s[i].o, off);
			int nlen = MAX(lx->win.s[y].staged->s[i].o + lx->win.s[y].staged->s[i].l, off + len) - noff;

			lx->win.s[y].staged->zl += (nlen - lx->win.s[y].staged->s[i].l);

			lx->win.s[y].staged->s[i].o = noff;
			lx->win.s[y].staged->s[i].l = nlen;

			break;
		}
	}

	if(type == LEFT || type == RIGHT)
	{
		if(len)
		{
			// reallocate if necessary
			if(lx->win.s[y].staged->a == lx->win.s[y].staged->l)
			{
				int ns = lx->win.s[y].staged->a ?: listwise_allocation_seed;
				ns = ns * 2 + ns / 2;

				fatal(xrealloc, &lx->win.s[y].staged->s, sizeof(*lx->win.s[0].staged->s), ns, lx->win.s[y].staged->a);
				lx->win.s[y].staged->a = ns;
			}

			// move down
			memmove(
					&lx->win.s[y].staged->s[i + 1]
				, &lx->win.s[y].staged->s[i]
				, (lx->win.s[y].staged->a - i - 1) * sizeof(lx->win.s[0].staged->s[0])
			);

			lx->win.s[y].staged->s[i].o = off;
			lx->win.s[y].staged->s[i].l = len;
			lx->win.s[y].staged->l++;
			lx->win.s[y].staged->zl += len;
		}
	}

	finally : coda;
}

/*
** These api's ensure that the next active/staged window will be at the other index
*/

int API lstack_window_unstage(lwx * const restrict lx, int y)
{
	lx->win.s[y].staged = 0;

	finally : coda;
}

int API lstack_window_deactivate(lwx * const restrict lx, int y)
{
	if(lx->win.s[y].active)
		lx->win.s[y].active->mark = 1;

	lx->win.s[y].active = 0;

	finally : coda;
}

int API lstack_windows_activate(lwx * const restrict lx)
{
	int y;
	LSTACK_ITERATE(lx, y, go)
	if(go)
	{
		if(lx->win.s[y].staged)
		{
			lx->win.s[y].active = lx->win.s[y].staged;
			lx->win.s[y].active_storage_index = lx->win.s[y].staged_storage_index;

			lx->win.s[y].staged = 0;

			// renew lease
			lx->win.s[y].active->lease = lx->win.active_era;

			// reset temp
			if(lx->s[0].t[y].y == LWTMP_WINDOW)
				lx->s[0].t[y].y = LWTMP_UNSET;
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
