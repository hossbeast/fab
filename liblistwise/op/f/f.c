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

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "liblistwise_control.h"

#include "macros.h"
#include "xmem.h"

/*

f operator - fields : window by fields

ARGUMENTS (1, or multiples of 2)
	 1  - offset, in fields, to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0
	*2  - length, in fields, of window
		    default : 0 - entire string

OPERATION

*/

static int op_validate(operation*);
static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "f"
		, .optype				= LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "window by fields"
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

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active && lx->win.s[x].active->lease == lx->win.active_era)
		{
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
					off = lx->win.s[x].active->l + win_off;
				if(win_len == 0)
					len = lx->win.s[x].active->l - off;
				else
					len = MIN(len, lx->win.s[x].active->l - off);

				if(off < lx->win.s[x].active->l)
				{
					int nlen = 0;

					int j;
					for(j = off; j < len; j++)
						nlen += lx->win.s[x].active->s[j].l;

					if(nlen > 0)
					{
						if(!wasreset)
						{
							// reset staged window, if any
							fatal(lstack_window_unstage, lx, x);
							wasreset = 1;

							// record this index was hit
							fatal(lstack_sel_stage, lx, x);
						}

						// append window segment
						fatal(lstack_window_stage, lx, x, lx->win.s[x].active->s[off].o, nlen);
					}
				}
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
