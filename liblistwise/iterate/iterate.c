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

#include "internal.h"

#define restrict __restrict

void API lwx_iterate_loop(lwx * const restrict lx, const int x, const int y, const int sel, int * const restrict go)
{
	*go = 1;
	if(sel && lx->sel.active && lx->sel.active->lease == lx->sel.active_era)
	{
		*go = 0;
		if(lx->sel.active->nil == 0 && lx->sel.active->sl > (y / 8))
		{
			*go = lx->sel.active->s[y / 8] & (0x01 << (y % 8));
		}
	}
}

int API lwx_lists(lwx * const restrict lx)
{
	return lx->l;
}

int API lwx_rows(lwx * const restrict lx, const int x)
{
	return lx->s[x].l;
}
