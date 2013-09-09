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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

/*

up operator - move selected entries to the head of the list and reset the selection

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. move selected entries to the head of the list
 2. select all entries

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "up"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_RESET
		, .op_exec			= op_exec
		, .desc					= "move selected entries to head of list"
	}, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(ls->sel.all == 0 && ls->sel.l != ls->s[0].l)
	{
		int i = 0;
		int x;
		LSTACK_ITERATE(ls, x, go)
		if(go)
		{
			typeof(ls->s[0].s[0]) T = ls->s[0].s[i];
			ls->s[0].s[i] = ls->s[0].s[x];
			ls->s[0].s[x] = T;
			i++;
		}
		LSTACK_ITEREND;
	}

	fatal(lstack_sel_all, ls);

	finally : coda;
}
