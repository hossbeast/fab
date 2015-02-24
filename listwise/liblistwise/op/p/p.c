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

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "macros.h"
#include "xlinux.h"

/*

p operator - partition : window by windows

ARGUMENTS (1, or multiples of 2)
	 1  - offset, in fields, to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0 - start of string
	*2  - length, in fields, of window
		    if negative, interpretted as offset from the end of the string
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
		, .mnemonic			= "partition"
		, .desc					= "window by windows"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl == 0 || (o->argsl != 1 && (o->argsl % 2) != 0))
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
		lwx_windows * win;
		int state;
		if((state = lstack_windows_state(lx, x, &win)) == LWX_WINDOWS_SOME)
		{
			char * ss = 0;
			int ssl = 0;
			int wasreset = 0;
			int wasread = 0;
			int i;
			for(i = 0; i < o->argsl; i += 2)
			{
				// { win_off, win_len } : offset and length, in windows, as specified
				int win_off = o->args[i]->i64;
				int win_len = 0;

				if(o->argsl > (i + 1))
					win_len = o->args[i + 1]->i64;

				// { off, len } : offset and length, in windows, after resolving negative values
				int off = win_off;
				int len = win_len;

				if(win_off < 0)
					off = (win->l + win_off) + 1;
				if(win_len == 0)
					len = win->l - off + 1;
				else if(win_len < 0)
					len = ((win->l + win_len) + 1) - off;

				if(off >= 0 && off <= win->l && len > 0)
				{
					if(!wasreset)
					{
						wasreset = 1;

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
						A = win->s[off].o + win->s[off].l;

					if(off + len < win->l)
					{
						B = win->s[off + len].o;
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
				
					fatal(lstack_windows_stage, lx, x, A, B - A);
				}
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
