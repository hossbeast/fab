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

#include "listwise/internal.h"


/*

dn operator - dirname : replace paths with dirname (path)
    not *quite* the same as libgen/dirname

NO ARGUMENTS

OPERATION

	1. foreach selected string
		2. replace that string with dirname(itself)

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "dn"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .mnemonic			= "dirname"
		, .desc					= "get component of filepath preceeding the filename"
	}
	, {}
};

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		int raw;

		// raw is true if this row is not an object entry, and has no window in effect
		fatal(lstack_readrow, ls, 0, x, &ss, &ssl, 0, 1, 1, 0, &raw);

		if(ssl)
		{
			char * s = ss;
			char * e = ss + ssl - 1;

			while(e != s && e[0] == '/')
				e--;

			if(e != s)
				e--;

			while(e != s && e[0] != '/')
				e--;

			while(e != s && e[0] == '/')
				e--;

			if(s != e || s[0] == '.')
			{
				e++;

				if(raw)
				{
					ls->s[0].s[x].l = e - s;
					ls->s[0].t[x].y = LWTMP_UNSET;
					ls->win.s[x].active = 0;
					ls->win.s[x].staged = 0;
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
	LSTACK_ITEREND

	finally : coda;
}
