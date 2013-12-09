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
#include "macros.h"

#define restrict __restrict

int API lstack_sel_stage(lwx * const restrict lx, int y)
{
	if(lx->sel.staged == 0)
	{
		lx->sel.staged = &lx->sel.storage[0];

		if(lx->sel.staged == lx->sel.active)
			lx->sel.staged = &lx->sel.storage[1];

		lx->sel.staged->l = 0;
		lx->sel.staged->sl = 0;
	}

	if(lx->sel.staged->lease != lx->sel.staged_era)
	{
		lx->sel.staged->l = 0;
		lx->sel.staged->sl = 0;
	}

	lx->sel.staged->nil = 0;
	lx->sel.staged->lease = lx->sel.staged_era;

	// reallocate if necessary
	if(lx->sel.staged->sa <= (y / 8))
	{
		fatal(xrealloc, &lx->sel.staged->s, sizeof(*lx->sel.staged->s), (y/8)+1, lx->sel.staged->sa);
		lx->sel.staged->sa = (y/8)+1;
	}

	// reset any already-allocated elements now visible
	int x;
	for(x = lx->sel.staged->sl; x <= (y/8); x++)
		lx->sel.staged->s[x] = 0;

	// increase length
	lx->sel.staged->sl = MAX(lx->sel.staged->sl, (y/8)+1);

	// set this particular bit
	if((lx->sel.staged->s[y/8] & (0x01 << (y%8))) == 0)
		lx->sel.staged->l++;

	lx->sel.staged->s[y/8] |= (0x01 << (y%8));

	finally : coda;
}

int API lstack_sel_unstage(lwx * const restrict lx)
{
	lx->sel.staged = 0;

	return 0;
}

int API lstack_sel_activate(lwx * const restrict lx)
{
	if(lx->sel.staged && lx->sel.staged->lease == lx->sel.staged_era)
	{
		lx->sel.active = lx->sel.staged;
	}
	else
	{
		lx->sel.active = &lx->sel.storage[0];
		lx->sel.active->nil = 1;
	}

	// renew lease
	lx->sel.active->lease = lx->sel.active_era;

	// reset staged
	lx->sel.staged = 0;

	return 0;
}

int API lstack_sel_reset(lwx * const restrict lx)
{
	lx->sel.active_era++;

	return 0;
}
