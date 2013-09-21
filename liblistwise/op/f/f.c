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

p operator - partition : window by fields (or characters, when no window in effect)

ARGUMENTS (1, or multiples of 2)
	 1  - offset, in fields, to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0
	*2  - length, in fields, of window
		    default : 0 - entire string

OPERATION

	1. if nothing selected, select all
	2. foreach selected row
    2.1. if no window is in effect, 
		2.2. replace that string with a slice of itself specified by the arguments

*/

static int op_validate(operation*);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "p"
		, .optype				= LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "window by fields or characters"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && (o->argsl % 2) != 0)
		fail("%s - %d arguments", o->op->s, o->argsl);

	int x;
	for(x = 0; x < o->argsl; x++)
	{
		if(o->args[x]->itype != ITYPE_I64)
			fail("%s - args[%d] should be i64", o->op->s, x);
	}

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	typeof(ls->s[0].w[0].s[0]) * wws = 0;
	int wl = 0;
	int wa = 0;

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(ls->s[0].w[x].y == 0)
		{
			char * ss = 0;
			int ssl   = 0;

			fatal(lstack_readrow, ls, 0, x, &ss, &ssl, 1, 0, 0, 0);

			int i;
			for(i = 0; i < o->argsl; i += 2)
			{
				int win_off = o->args[i]->i64;
				int win_len = 0;

				if(o->argsl > i)
					win_len = o->args[i + 1]->i64;

				int off = win_off;
				int len = win_len;

				if(win_off < 0)
					off = ssl + win_off;
				if(win_len == 0)
					len = ssl - off;
				else
					len = MIN(len, ssl - off);

				if(off < ssl && len > 0)
				{
					// append window segment
					fatal(lstack_window_add, ls, 0, x, off, len);

					// record this index was hit
					fatal(lstack_sel_stage, ls, x);
				}
			}
		}
		else
		{
			// original windows length
			typeof(ls->s[0].w[0].s[0]) * ws = ls->s[0].w[x].s;
			wl = ls->s[0].w[x].l;

			int wasreset = 0;
			int i;
			for(i = 0; i < o->argsl; i += 2)
			{
				int win_off = o->args[i]->i64;
				int win_len = 0;

				if(o->argsl > i)
					win_len = o->args[i + 1]->i64;

				int off = win_off;
				int len = win_len;

				if(win_off < 0)
					off = wl + win_off;
				if(win_len == 0)
					len = wl - off;
				else
					len = MIN(len, wl - off);

				if(off < wl)
				{
					int nlen = 0;

					int j;
					for(j = off; j < len; j++)
						nlen += ws[j].l;

					if(nlen > 0)
					{
						if(!wasreset)
						{
							// take a copy of the window for this row
							if(wl > wa)
							{
								fatal(xrealloc, &wws, sizeof(*ws), wl, wa);
								wa = wl;
							}
							memcpy(wws, ls->s[0].w[x].s, sizeof(*ws) * wl);
							ws = wws;

							// reset the window
							fatal(lstack_window_reset, ls, 0, x);
							wasreset = 1;
						}

						// append window segment
						fatal(lstack_window_stage, ls, 0, x, ws[off].o, nlen);

						// record this index was hit
						fatal(lstack_sel_stage, ls, x);
					}
				}
			}
		}
	}
	LSTACK_ITEREND

finally:
	free(wws);
coda;
}
