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
#include <alloca.h>

#include "listwise/internal.h"

/*

x operator - extract window

NO ARGUMENTS

OPERATION

	2. foreach selected string
		3. replace that string the concatenation of its windowed segments

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "x"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_OPERATION_INPLACE | LWOP_ARGS_CANHAVE
		, .op_exec			= op_exec
		, .desc					= "extract row window"
	}
	, {}
};

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	// delimiter string
	char * ds = 0;
	int dl = 0;
	if(o->argsl)
	{
		ds = o->args[0]->s;
		dl = o->args[0]->l;
	}

	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active && lx->win.s[x].active->lease == lx->win.active_era)
		{
			char * zs;
			int    zsl;

			if(ds)
			{
				// clearing/appending will reset the windows
				typeof(*lx->win.s[0].active) * win = lx->win.s[x].active;

				// the str parameter will cause readrow to return the temp space for the row
				fatal(lstack_readrow, lx, 0, x, &zs, &zsl, 0, 1, 0, 1, 0);

				// clear this row
				fatal(lstack_clear, lx, 0, x);

				int i;
				for(i = 0; i < win->l; i++)
				{
					if(i)
						fatal(lstack_append, lx, 0, x, ds, dl);

					fatal(lstack_append, lx, 0, x, zs + win->s[i].o, win->s[i].l);
				}
			}
			else
			{
				// because there is a window in effect, getbytes returns the temp space for the row
				fatal(lstack_getbytes, lx, 0, x, &zs, &zsl);

				// write new string using the window
				fatal(lstack_write, lx, 0, x, zs, zsl);

				// record this index was hit
				fatal(lstack_sel_stage, lx, x);
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
