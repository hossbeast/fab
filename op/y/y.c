/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"
#include "xmem.h"

/*

y operator - select those items affected by the last operation

NO ARGUMENTS

OPERATION

	1. select those items affected by the preceeding operator

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "y"
		, .optype				= LWOP_SELECTION_RESET
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "select entries affected by preceeding operator"
	}, {}
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(ls->last.l == ls->s[0].l)
	{
		ls->sel.all = 1;
	}
	else
	{
		if(ls->sel.sa < ls->last.sl)
		{
			free(ls->sel.s);
			if((ls->sel.s = calloc(1, ls->last.sl * sizeof(*ls->sel.s))) == 0)
				return 0;

			ls->sel.sa = ls->last.sl;
		}

		memcpy(
				ls->sel.s
			, ls->last.s
			, ls->last.sl * sizeof(ls->sel.s[0])
		);
		ls->sel.l = ls->last.l;
		ls->sel.sl = ls->last.sl;
		ls->sel.all = 0;
	}

	return 1;
}





