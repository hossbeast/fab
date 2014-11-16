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

int API lstack_selection_stage(lwx * const restrict lx, int y)
{
	if(lx->sel.staged == 0)
	{
		lx->sel.staged = &lx->sel.storage[0];

		if(lx->sel.staged == lx->sel.active)
			lx->sel.staged = &lx->sel.storage[1];

		// force renewal
		lx->sel.staged->lease = -1;
	}

	// renew lease
	if(lx->sel.staged->lease != lx->sel.staged_era || lx->sel.staged->nil)
	{
		lx->sel.staged->lease = lx->sel.staged_era;
		lx->sel.staged->nil = 0;
		lx->sel.staged->l = 0;
		lx->sel.staged->sl = 0;
	}

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

int API lstack_selection_activate(lwx * const restrict lx)
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

	finally : coda;
}

int API lstack_selection_reset(lwx * const restrict lx)
{
	lx->sel.active_era++;

	finally : coda;
}

int API lstack_selection_state(lwx * const restrict lx)
{
	if(lx->sel.active == 0 || lx->sel.active->lease != lx->sel.active_era)
	{
		return LWX_SELECTED_ALL;		// all rows are selected
	}
	else if(lx->sel.active->nil || lx->sel.active->l == 0)
	{
		return LWX_SELECTED_NONE;		// no rows selected
	}
	else if(lx->sel.active->l == lx->s[0].l)
	{
		return LWX_SELECTED_SOME;		// subset of rows are selected
	}
	else
	{
		return LWX_SELECTED_ALL;		// all rows are selected
	}
}
