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

#include "control.h"

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
		, .optype				= LWOP_SELECTION_READ | LWOP_OPERATION_INPLACE | LWOP_OBJECT_NO
		, .op_exec			= op_exec
		, .desc					= "get component of filepath following the last slash"
	}
	, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int x;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));
		}

		if(go)
		{
			if(ls->s[0].s[x].l && ls->s[0].s[x].type == 0)
			{
				char * o = ls->s[0].s[x].s;
				char * s = ls->s[0].s[x].s;
				char * e = ls->s[0].s[x].s + ls->s[0].s[x].l - 1;

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
							memmove(
									ls->s[0].s[x].s
								, s
								, e - s
							);

							ls->s[0].s[x].l = e - s;
							ls->s[0].t[x].w = 0;

							// record this index was hit
							fatal(lstack_last_set, ls, x);
						}
					}
				}
			}
		}
	}

	finally : coda;
}
