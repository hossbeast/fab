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

#include "liblistwise_control.h"

/*

c operator - coalesce lists on the stack

NO ARGUMENTS
 1*. from - merge lists starting at this index, and counting down, into {to} list
            if negative, interpreted as offset from number of lists
              i.e. -1 refers to highest-numbered list index
            default : -1
 2*. to   - default : 0

OPERATION

 1. coalesce all lists onto the top list
 2. clear the current selection

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "c"
		, .optype				= LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "coalesce lists on the stack"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl)
	{
		if(o->args[0]->itype != ITYPE_I64)
			fail("c -- first argument should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int from = -1;

	if(o->argsl)
		from = o->args[0]->i64;

	if(from < 0)
		from = ls->l + from;

	int x;
	for(x = from; x >= 1; x--)
		fatal(lstack_merge, ls, 0, x);

	fatal(lstack_sel_all, ls);

	finally : coda;
}
