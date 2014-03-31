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

#include "macros.h"
#include "xlinux.h"

/*

p operator - partition : window by fields

ARGUMENTS (1, or multiples of 2)
	 1  - offset, in fields, to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0
	*2  - length, in fields, of window
		    default : 0 - entire string

OPERATION

*/

static int op_validate(operation*);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "p"
		, .optype				= LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "fields"
		, .desc					= "window by fields"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && (o->argsl % 2) != 0)
		failf(LW_ARGSNUM, "actual : %d", o->argsl);

	int x;
	for(x = 0; x < o->argsl; x++)
	{
		if(o->args[x]->itype != ITYPE_I64)
			fails(LW_ARGSTYPE, "should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active && lx->win.s[x].active->lease == lx->win.active_era)
		{
			char * ss = 0;
			int ssl = 0;
			int wasreset = 0;
			int wasread = 0;
			int i;
			for(i = 0; i < o->argsl; i += 2)
			{
				int win_off = o->args[i]->i64;
				int win_len = 0;

				if(o->argsl > (i + 1))
					win_len = o->args[i + 1]->i64;

				int off = win_off;
				int len = win_len;

				if(win_off < 0)
					off = (lx->win.s[x].active->l + win_off) + 1;
				if(win_len == 0)
					len = lx->win.s[x].active->l - off + 1;
				else
					len = MIN(len, (lx->win.s[x].active->l + 1) - off);

				if(off >= 0 && off < (lx->win.s[x].active->l + 1) && len > 0)
				{
					if(!wasreset)
					{
						wasreset = 1;

						// reset staged window, if any
						fatal(lstack_window_unstage, lx, x);

						// record this index was hit
						fatal(lstack_selection_stage, lx, x);
					}

					// append window segment
					int A = 0;
					int B;
					if(off == 0)
						A = 0;
					off--;

					if(off > -1)
						A = lx->win.s[x].active->s[off].o + lx->win.s[x].active->s[off].l;

					if(off + len < lx->win.s[x].active->l)
					{
						B = lx->win.s[x].active->s[off + len].o;
					}
					else
					{
						if(!wasread)
						{
							wasread = 1;
							fatal(lstack_readrow, lx, 0, x, &ss, &ssl, 0, 1, 0, 0, 0);
						}

						B = ssl;
					}
				
					fatal(lstack_window_stage, lx, x, A, B - A);
				}
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
