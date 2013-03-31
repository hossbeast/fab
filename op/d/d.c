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

/*

d operator - sunDer selected (Delete)

NO ARGUMENTS

OPERATION

	1. push an empty list on the stack
	2. invert the current selection
	3. move all selected items to the top list

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_RESET
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "extract selected items into a new list"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	// create a new list at index 0
	fatal(lstack_unshift, ls);

	if(!ls->sel.all && ls->sel.l != ls->s[1].l)
	{
		// ensure allocation in the new list @ [0]
		fatal(lstack_ensure, ls, 0, ls->s[1].l - ls->sel.l - 1, -1);

		int i = 0;
		int x;
		for(x = ls->s[1].l - 1; x >= 0; x--)
		{
			if((ls->sel.sl <= (x/8)) || (ls->sel.s[x/8] & (0x01 << (x%8))) == 0)
			{
				fatal(lstack_move, ls, 0, ls->s[1].l - ls->sel.l - 1 - i, 1, x);
			}
		}
	}

	fatal(lstack_sel_all, ls);

	finally : coda;
}





