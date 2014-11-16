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

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "macros.h"
#include "xlinux.h"

/*

wvf operator - window following

NO ARGUMENTS

*/

static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "wvf"
		, .optype				= LWOP_WINDOWS_ACTIVATE
		, .op_exec			= op_exec
		, .desc					= "window following"
	}
	, {}
};

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
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
				// reset staged windows
//				fatal(lstack_window_unstage, lx, x);

				// read the row
				int ssl = 0;
				fatal(lstack_readrow, lx, 0, x, 0, &ssl, 0, 1, 0, 0, 0);

				// following the last windowed segment
				fatal(lstack_window_stage, lx, x, ws[wl - 1].o + ws[wl - 1].l, ssl - (ws[wl - 1].o + ws[wl - 1].l));
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
