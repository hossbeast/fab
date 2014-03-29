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

#include "internal.h"

#include "color.h"
#include "macros.h"

/*

hl operator - highlight window within each row with terminal color escapes

ARGUMENTS
	 1  - offset to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0
	*2  - length of window
		    default : 0 - entire string

OPERATION

	1. if nothing selected, select all
	2. foreach selected string
		3. replace that string with a slice of itself specified by the arguments

*/

static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "hl"
		, .optype				= LWOP_OPERATION_INPLACE | LWOP_WINDOWS_ACTIVATE
		, .op_exec			= op_exec
		, .mnemonic			= "highlight"
		, .desc					= "highlight windows"
	}
	, {}
};

static int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active && lx->win.s[x].active->lease == lx->win.active_era)
		{
			// clearing/appending will reset the windows
			typeof(*lx->win.s[0].active) * win = lx->win.s[x].active;

			// request that readrow return temp space
			char * zs;
			int    zsl;
			fatal(lstack_readrow, lx, 0, x, &zs, &zsl, 0, 1, 0, 1, 0);

			// clear this string on the stack
			fatal(lstack_clear, lx, 0, x);

			// cumulative offset within the new string
			int z = 0;

			// text in the subject before the first windowed segment
			fatal(lstack_append, lx, 0, x, zs, win->s[0].o);
			z += win->s[0].o;

			int i;
			for(i = 0; i < win->l; i++)
			{
				// text following the last segment, and preceeding this segment
				if(i)
				{
					fatal(lstack_append, lx, 0, x, zs + win->s[i - 1].o + win->s[i - 1].l, win->s[i].o - (win->s[i - 1].o + win->s[i - 1].l));
					z += win->s[i].o - (win->s[i - 1].o + win->s[i - 1].l);
				}

				// write the windowed segment bracketed by color escapes
				fatal(lstack_append, lx, 0, x, COLOR(RED));
				z += CSIZE(RED);
				fatal(lstack_append, lx, 0, x, zs + win->s[i].o, win->s[i].l);
				fatal(lstack_append, lx, 0, x, COLOR(NONE));

				fatal(lstack_window_stage, lx, x, z, win->s[i].l);

				z += win->s[i].l + CSIZE(NONE);
			}

			// text in the subject following the last windowed segment
			if(i)
			{
				i--;
				fatal(lstack_append, lx, 0, x, zs + win->s[i].o + win->s[i].l, zsl - (win->s[i].o + win->s[i].l));
			}

			fatal(lstack_sel_stage, lx, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
