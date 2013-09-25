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
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "liblistwise_control.h"

/*

x operator - extract window

NO ARGUMENTS

OPERATION

	2. foreach selected string
		3. replace that string the concatenation of its windowed segments

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "x"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .desc					= "extract row window"
	}
	, {}
};

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		if(lx->win.s[x].active)
		{
			// because there is a window in effect, getbytes returns the temp space for the row
			char * zs;
			int    zsl;
			fatal(lstack_getbytes, lx, 0, x, &zs, &zsl);

			// write new string using the window
			fatal(lstack_write, lx, 0, x, zs, zsl);

			// record this index was hit
			fatal(lstack_sel_stage, lx, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
