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

#include "xlinux.h"

/*

t operator - tear windowed segments into new rows

NO ARGUMENTS

OPERATION

 use current select, ELSE
 use entire top list

 for each entry
	1. create N copies of this entry at indexes x + 1 .. N

*/

static xapi op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "t"
		, .optype				= LWOP_OPERATION_PUSHBEFORE | LWOP_SELECTION_RESET
		, .op_exec			= op_exec
		, .desc					= "tear windows into a new list"
		, .mnemonic			= "tear"
	}, {}
};

static xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
  enter;

	fatal(lstack_unshift, lx);

	int x;
	int i;

	LSTACK_ITERATE_FWD(lx, 1, x, 1, 1, go)
	if(go)
	{
		struct lwx_windows * win;
		int state;
		if((state = lstack_windows_state(lx, x, &win)) != LWX_WINDOWS_NONE)
		{
			// request that readrow return temp space, and not to resolve the active window
			char * zs;
			int    zsl;
			fatal(lstack_readrow, lx, 1, x, &zs, &zsl, 0, 1, 0, 1, 0);

			if(state == LWX_WINDOWS_ALL)
			{
				fatal(lstack_addw, lx, zs, zsl);
			}
			else
			{
				for(i = 0; i < win->l; i++)
				{
					// write the windowed segment
					fatal(lstack_addw, lx, zs + win->s[i].o, win->s[i].l);
				}
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
