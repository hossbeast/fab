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

#include <listwise/operator.h>

#include "control.h"

/*

shift operator - delete the 0th list from the stack

ARGUMENTS
	[1] - number of times to shift, default : 1

OPERATION

 N times (0 means until only the Nth list remains)
	 1. delete the 0th list from the stack

*/

static int op_exec(operation*, lstack*, int**, int*);
static int op_validate(operation*);

operator op_desc[] = {
	{
		  .s						= "shift"
		, .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE
		, .op_exec			= op_exec
		, .op_validate	= op_validate
		, .desc					= "delete the 0th list"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl && o->args[0]->itype != ITYPE_I64)
		fail("shift -- expected first argument : i64");

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int N = 1;
	if(o->argsl)
		N = o->args[0]->i64;
	if(N == 0)
		N = ls->l - 1;

	int x;
	for(x = 0; x < N; x++)
		fatal(lstack_shift, ls);

	// if anything was selected, its now used up
	fatal(lstack_sel_all, ls);

	finally : coda;
}
