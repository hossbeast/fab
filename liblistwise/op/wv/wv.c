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

#include "macros.h"
#include "xlinux.h"

/*

wv operator - invert windows

NO ARGUMENTS

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "wv"
		, .optype				= LWOP_WINDOWS_ACTIVATE
		, .op_exec			= op_exec
		, .desc					= "invert windows"
	}
	, {}
};

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active && lx->win.s[x].active->lease == lx->win.active_era)
		{
			typeof(lx->win.s[0].active->s[0]) * ws = lx->win.s[x].active->s;
			int wl = lx->win.s[x].active->l;

			if(wl)
			{
				// reset staged windows, if any
				fatal(lstack_window_unstage, lx, x);

				char * s = 0;
				int sl = 0;
				fatal(lstack_readrow, lx, 0, x, &s, &sl, 0, 1, 0, 0, 0);

				// before the first windowed segment
				fatal(lstack_window_stage, lx, x, 0, ws[0].o);

				int i;
				for(i = 1; i < wl; i++)
				{
					// region following the previous segment and preceeding the current segment
					fatal(lstack_window_stage, lx, x, ws[i - 1].o + ws[i - 1].l, ws[i].o - (ws[i - 1].o + ws[i - 1].l));
				}

				// following the last windowed segment
				fatal(lstack_window_stage, lx, x, ws[i - 1].o + ws[i - 1].l, sl - (ws[i - 1].o + ws[i - 1].l));
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
