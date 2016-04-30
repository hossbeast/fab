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

#include <stdlib.h>
#include <string.h>

#include "xlinux.h"

#include "internal.h"
#include "selection.internal.h"
#include "lstack.internal.h"
#include "lwx.internal.h"

#include "macros.h"

#define restrict __restrict

//
// api
//

API xapi lstack_selection_stage(lwx * const restrict lx, int y)
{
	enter;

	if(lx->sel.staged == 0)
	{
		lx->sel.staged = &lx->sel.storage[0];

		if(lx->sel.staged == lx->sel.active)
			lx->sel.staged = &lx->sel.storage[1];

		// force renewal
		lx->sel.staged->lease = -1;
	}

	// renew lease
	if(lx->sel.staged->lease != lx->sel.staged_era || lx->sel.staged->state == LWX_SELECTION_NONE)
	{
		lx->sel.staged->lease = lx->sel.staged_era;
		lx->sel.staged->state = 0;
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

	// set this particular bit, track total bits
	if((lx->sel.staged->s[y/8] & (0x01 << (y%8))) == 0)
	{
		if(++lx->sel.staged->l == lx->s[0].l)
			lx->sel.staged->state = LWX_SELECTION_ALL;
	}

	lx->sel.staged->s[y/8] |= (0x01 << (y%8));

	finally : coda;
}

API xapi lstack_selection_reset(lwx * const restrict lx)
{
	enter;

	lx->sel.active_era++;

	finally : coda;
}

API int lstack_selection_state(lwx * const restrict lx)
{
	if(lx->sel.active == 0 || lx->sel.active->lease != lx->sel.active_era)
	{
		return LWX_SELECTION_ALL;		// all rows are selected
	}

	return lx->sel.active->state;
}

//
// public
//

xapi lstack_selection_activate(lwx * const restrict lx)
{
	enter;

	if(lx->sel.staged && lx->sel.staged->lease == lx->sel.staged_era)
	{
		lx->sel.active = lx->sel.staged;
	}
	else
	{
		// no selection made ; nil selection
		lx->sel.active = &lx->sel.storage[0];
		lx->sel.active->state = LWX_SELECTION_NONE;
	}

	// renew lease
	lx->sel.active->lease = lx->sel.active_era;

	// reset staged
	lx->sel.staged = 0;

	finally : coda;
}
