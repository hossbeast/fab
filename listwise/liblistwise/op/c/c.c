/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include "listwise/operator.h"
#include "listwise/logging.h"
#include "listwise/lwx.h"

/*

c operator - coalesce lists on the stack into the top list

ARGUMENTS
 1*. from - merge lists starting at this index, and counting down, into {to} list
            if negative, interpreted as offset from number of lists
              i.e. -1 refers to highest-numbered list index
            default : -1

ARGUMENTS
 [0] : N - merge lists from indexes [ 1, N ] inclusive into list 0

default : 1, i.e. merge list[1] into list[0]

negative N counts indexes starting from the highest-numbered list
N = 0 means merge all lists

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "c"
		, .optype				= LWOP_ARGS_CANHAVE | LWOP_SELECTION_RESET | LWOP_STACKOP
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "combine lists on the stack"
		, .mnemonic			= "coalesce"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl)
	{
		if(o->args[0]->itype != ITYPE_I64)
			fails(LW_ARGSTYPE, "should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int N = 1;

	if(o->argsl)
		N = o->args[0]->i64;

	if(N == 0)
		N = ls->l - 1;
	else if(N < 0)
		N = ls->l + N;

	if(N < 0 || N >= ls->l)
	{
		lw_log_opinfo("c:N=%d is out-of-bounds %d", N, ls->l);
	}
	else
	{
		int x;
		for(x = 0; x < N; x++)
			fatal(lstack_merge, ls, 0, 1);
	}

	finally : coda;
}
