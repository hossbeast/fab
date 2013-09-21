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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

#include "macros.h"
#include "xmem.h"

/*

vw operator - invert windows

NO ARGUMENTS

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "vw"
		, .optype				= LWOP_WINDOWS_ACTIVATE | LWOP_WINDOWS_READ
		, .op_exec			= op_exec
		, .desc					= "invert windows"
	}
	, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	typeof(ls->s[0].w[0].s[0]) * ws = 0;
	int wl = 0;
	int wa = 0;

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(ls->s[0].w[x].y == 0 || ls->s[0].w[x].l == 0)
		{
			fatal(lstack_window_stage, ls, x, 0, 0);
		}
		else
		{
			// take a copy of the window for this row
			wl = ls->s[0].w[x].l;
			if(wl > wa)
			{
				fatal(xrealloc, &ws, sizeof(*ws), wl, wa);
				wa = wl;
			}
			memcpy(ws, ls->s[0].w[x].s, sizeof(*ws) * wl);

			// reset the window
			fatal(lstack_window_reset, ls, 0, x);

			if(wl)
			{
				char * s = 0;
				int sl = 0;
				fatal(lstack_readrow, ls, 0, x, &s, &sl, 1, 0, 0, 0);

				// before the first windowed segment
				fatal(lstack_window_stage, ls, x, 0, ws[0].o);

				int i;
				for(i = 1; i < wl; i++)
				{
					if(off < wl)
					{
						int nlen = 0;

						int j;
						for(j = off; j < len; j++)
							nlen += ws[j].l;

						if(nlen > 0)
						{
							// append window segment
							fatal(lstack_window_stage, ls, 0, x, ws[off].o, nlen);

							// record this index was hit
							fatal(lstack_sel_stage, ls, x);
						}
					}
				}

				// following the last windowed segment
				fatal(lstack_window_stage, ls, x, ws[i].o + ws[i].l, 
			}
		}
	}
	LSTACK_ITEREND

finally:
	free(ws);
coda;
}
