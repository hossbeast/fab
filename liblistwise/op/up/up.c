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

#include "listwise/internal.h"

/*

up operator - move selected entries to the head of the list and reset the selection

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. move selected entries to the head of the list
 2. select all entries

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "up"
		, .optype				= LWOP_SELECTION_RESET | LWOP_WINDOWS_RESET | LWOP_SELECTION_STAGE
		, .op_exec			= op_exec
		, .mnemonic			= "up"
		, .desc					= "move rows to the top"
	}, {}
};

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	if(lx->sel.active == 0 || lx->sel.active->lease != lx->sel.active_era)
	{
		// everything selected ; no-op
	}
	else if(lx->sel.active->nil)
	{
		// nothing selected ; no-op
	}
	else
	{
		int i = 0;
		int x;
		LSTACK_ITERATE(lx, x, go)
		if(go)
		{
			typeof(lx->s[0].s[0]) Ts = lx->s[0].s[i];
			lx->s[0].s[i] = lx->s[0].s[x];
			lx->s[0].s[x] = Ts;

			typeof(lx->s[0].t[0]) Tt = lx->s[0].t[i];
			lx->s[0].t[i] = lx->s[0].t[x];
			lx->s[0].t[x] = Tt;

			i++;
		}
		LSTACK_ITEREND;

		for(x = 0; x < i; x++)
			fatal(lstack_sel_stage, lx, x);
	}

	finally : coda;
}
