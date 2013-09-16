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

bn operator - basename : replace paths with basename(path) (see basename)

NO ARGUMENTS

OPERATION

	1. foreach selected string
		2. replace that string with basename(itself)

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "bn"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .desc					= "get component of filepath following the last slash"
	}
	, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		int raw;

		// raw is true if this row is not an object entry, and has no window in effect
		fatal(lstack_readrow, ls, 0, x, &ss, &ssl, 1, 1, 0, &raw);

		if(ssl)
		{
			char * o = ss;
			char * s = ss;
			char * e = ss + ssl - 1;

			while(e != s && e[0] == '/')
				e--;

			if((s = e) != o)
				s--;

			while(s != o && s[0] != '/')
				s--;

			if(s != e)
			{
				e++;
				if(s[0] == '/')
					s++;

				if(e - s != 1 || s[0] != '.')
				{
					if(e - s != 2 || s[0] != '.' || s[1] != '.')
					{
						if(raw)
						{
							memmove(
									ls->s[0].s[x].s
								, s
								, e - s
							);

							ls->s[0].s[x].l = e - s;
							ls->s[0].w[x].y = 0;
							ls->s[0].t[x].y = 0;
						}
						else
						{
							// rewrite the entry
							fatal(lstack_write, ls, 0, x, s, e - s);
						}

						// record this index was hit
						fatal(lstack_sel_stage, ls, x);
					}
				}
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
