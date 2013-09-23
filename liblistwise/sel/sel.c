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

int API lstack_sel_reset(lwx * const restrict lx)
{
	lx->sel.active = 0;

	return 0;
}

int API lstack_sel_unstage(lwx * const restrict lx)
{
	lx->sel.staged = 0;

	return 0;
}

int API lstack_sel_stage(lstack* const restrict ls, int y)
{
	// reallocate if necessary
	if(ls->stage.sa <= (y / 8))
	{
		fatal(xrealloc, &ls->stage.s, sizeof(ls->stage.s[0]), (y/8)+1, ls->stage.sa);
		ls->stage.sa = (y/8)+1;
	}

	// reset any already-allocated elements now visible
	int x;
	for(x = ls->stage.sl; x <= (y/8); x++)
		ls->stage.s[x] = 0;

	// increase length
	ls->stage.sl = MAX(ls->stage.sl, (y/8)+1);

	// set this particular bit
	if((ls->stage.s[y/8] & (0x01 << (y%8))) == 0)
		ls->stage.l++;

	ls->stage.s[y/8] |= (0x01 << (y%8));

	finally : coda;
}

int API lstack_sel_activate(lwx * const restrict lx)
{
	lx->sel.active = lx->sel.staged;

	if(ls->stage.l == ls->s[0].l)
	{
		ls->sel.all = 1;
	}
	else
	{
		if(ls->sel.sa < ls->stage.sl)
		{
			fatal(xrealloc, &ls->sel.s, sizeof(*ls->sel.s), ls->stage.sl, ls->sel.sa);
			ls->sel.sa = ls->stage.sl;
		}

		memcpy(
				ls->sel.s
			, ls->stage.s
			, ls->stage.sl * sizeof(ls->sel.s[0])
		);
		ls->sel.l = ls->stage.l;
		ls->sel.sl = ls->stage.sl;
		ls->sel.all = 0;
	}

	finally : coda;
}
