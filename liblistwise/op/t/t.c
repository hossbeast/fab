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

#include "listwise/internal.h"

#include "xmem.h"

/*

t operator - tear windowed segments into new rows

NO ARGUMENTS

OPERATION

 use current select, ELSE
 use entire top list

 for each entry
	1. create N copies of this entry at indexes x + 1 .. N

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "t"
		, .optype				= LWOP_OPERATION_PUSHBEFORE
		, .op_exec			= op_exec
		, .desc					= "tear windows into a new list"
		, .mnemonic			= "tear"
	}, {}
};

static int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	fatal(lstack_unshift, lx);

	int x;
	LSTACK_ITERATE_FWD(lx, 1, x, 1, go)
	if(go)
	{
		if(lx->win.s[x].active && lx->win.s[x].active->lease == lx->win.active_era)
		{
			typeof(lx->win.s[0].active->s[0]) * ws = lx->win.s[x].active->s;
			int wl = lx->win.s[x].active->l;

			// request that readrow return temp space, and not to resolve the active window
			char * zs;
			int    zsl;
			fatal(lstack_readrow, lx, 1, x, &zs, &zsl, 0, 1, 0, 1, 0);

			int i;
			for(i = 0; i < wl; i++)
			{
				// write the windowed segment
				fatal(lstack_add, lx, zs + ws[i].o, ws[i].l);
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
