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

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "hl"
		, .optype				= LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .mnemonic			= "highlight"
		, .desc					= "highlight windows"
	}
	, {}
};

static int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active)
		{
			// because there is a window in effect, getbytes must return the temp space for the row
			char * zs;
			int    zsl;
			fatal(lstack_getbytes, lx, 0, x, &zs, &zsl);

			// clear this string on the stack
			lstack_clear(lx, 0, x);

			// text in the subject before the first windowed segment
			fatal(lstack_append, lx, 0, x, zs, lx->win.s[x].active->s[0].o);

			int i;
			for(i = 0; i < lx->win.s[x].active->l; i++)
			{
				// write the windowed segment bracketed by color escapes
				fatal(lstack_append, lx, 0, x, COLOR(RED));
				fatal(lstack_append, lx, 0, x, zs + lx->win.s[x].active->s[i].o, lx->win.s[x].active->s[i].l);
				fatal(lstack_append, lx, 0, x, NOCOLOR);
			}

			// text in the subject following the last windowed segment
			if(i)
			{
				i--;
				fatal(lstack_append, lx, 0, x, zs + lx->win.s[x].active->s[i].o + lx->win.s[x].active->s[i].l, zsl - (lx->win.s[x].active->s[i].o + lx->win.s[x].active->s[i].l));
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
