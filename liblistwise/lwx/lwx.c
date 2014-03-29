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

#include "internal.h"

#include "xlinux.h"

#define restrict __restrict

int API lwx_alloc(lwx ** const lx)
{
	fatal(xmalloc, lx, sizeof(**lx));

	finally : coda;
}

uint64_t API lwx_getflags(lwx * const lx)
{
	return lx->flags;
}

uint64_t API lwx_setflags(lwx * const lx, const uint64_t g)
{
	return ((lx->flags = g));
}

typedef void * voidstar;
voidstar API lwx_getptr(lwx * const lx)
{
	return lx->ptr;
}

voidstar API lwx_setptr(lwx * const lx, void * const g)
{
	return ((lx->ptr = g));
}

void API lwx_free(lwx * lx)
{
	if(lx && lx != listwise_identity)
	{
		int x;
		int y;
		for(x = 0; x < lx->a; x++)
		{
			for(y = 0; y < lx->s[x].a; y++)
			{
				free(lx->s[x].s[y].s);
				free(lx->s[x].t[y].s);

				if(x == 0)
				{
					free(lx->win.s[y].storage[0].s);
					free(lx->win.s[y].storage[1].s);
				}
			}

			free(lx->s[x].s);
			free(lx->s[x].t);
		}

		free(lx->s);
		free(lx->win.s);
		free(lx->sel.storage[0].s);
		free(lx->sel.storage[1].s);
		free(lx);
	}
}

void API lwx_xfree(lwx ** lx)
{
	lwx_free(*lx);
	*lx = 0;
}

int API lwx_reset(lwx * lx)
{
	int x;
	for(x = 0; x < lx->l; x++)
	{
		int y;
		for(y = 0; y < lx->s[x].l; y++)
			fatal(lstack_clear, lx, x, y);

		lx->s[x].l = 0;
	}

	lx->l = 0;
	lx->sel.active = 0;
	lx->sel.staged = 0;

	finally : coda;
}
