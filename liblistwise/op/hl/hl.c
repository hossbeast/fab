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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

#include "color.h"
#include "macros.h"

/*

hlm operator - highlight regex matches within each entry with terminal color escapes
hlw operator - highlight regex matches within each entry with terminal color escapes

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

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "hl"
		, .optype				= LWOP_SELECTION_READ | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .desc					= "highlight windows"
	}
	, {}
};

static int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	char * ss = 0;
	int ssa = 0;

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		// copy of the starting string
		char * zs;
		int ssl;
		fatal(lstack_string, ls, 0, x, &zs, &ssl);
		
		if(ssl >= ssa)
		{
			fatal(xrealloc, &ss, 1, ssl + 1, ssa);
			ssa = ssl + 1;
		}
		memcpy(ss, zs, ssl);
		ss[ssl] = 0;

		// clear this string on the stack
		lstack_clear(ls, 0, x);

		// text in the subject before the first segment
		fatal(lstack_append, ls, 0, x, ss, ls->s[0].w[x].o);

		int i;
		for(i = 0; i < ls->s[0].w[x].l; i++)
		{
			// write new string using the window
			fatal(lstack_append, ls, 0, x, COLOR(RED));
			fatal(lstack_append, ls, 0, x, ss + ls->s[0].w[x].o, ls->s[0].w[x].l);
			fatal(lstack_append, ls, 0, x, NOCOLOR);
		}

		// text in the subject following the last segment
		if(i)
		{
			i--;
			fatal(lstack_append, ls, 0, x, ss + ls->s[0].w[i].o + ls->s[0].w[i].l, ssl - (ls->s[0].w[i].o + ls->s[0].w[i].l, ssl));
		}
	}
	LSTACK_ITEREND

finally:
	free(ss);
coda;
}
