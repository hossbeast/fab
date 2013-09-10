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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

/*

x operator - extract window

NO ARGUMENTS

OPERATION

	2. foreach selected string
		3. replace that string the concatenation of its windowed segments

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "x"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .desc					= "extract row window"
	}
	, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(ls->s[0].w[x].y)
		{
			// because there is a window in effect, getbytes must return the temp space for the row
			char * zs;
			int    zsl;
			fatal(lstack_getybtes, ls, 0, x, &zs, &zsl);

			// clear this string on the stack
			lstack_clear(ls, 0, x);

			// write new string using the window
			fatal(lstack_write, ls, 0, x, zs, zsl);

			// record this index was hit
			fatal(lstack_last_set, ls, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
