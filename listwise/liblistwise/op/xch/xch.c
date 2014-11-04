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

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"
#include "listwise/logging.h"

/*

xch operator - eXCHange lists

ARGUMENTS
 1*. to - exchange top list with list at this index
            if negative, interpreted as offset from number of lists
              i.e. -1 refers to highest-numbered list index
            default : 1

OPERATION

	1. exchange the top list and the {to} list

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "xch"
		, .optype				= LWOP_ARGS_CANHAVE | LWOP_STACKOP
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "swap a list to the top of the stack"
		, .mnemonic			= "exchange"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl)
	{
		if(o->args[0]->itype != ITYPE_I64)
			fails(LW_ARGSDOM, "should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int to = 1;
	if(o->argsl)
		to = o->args[0]->i64;
	if(to < 0)
		to = ls->l + to;

	if(!(ls->l > to))
	{
		lw_log_opinfo("xch/%d out of range", to);
	}
	else
	{
		typeof(ls->s[0]) T = ls->s[0];
		ls->s[0] = ls->s[to];
		ls->s[to] = T;
	}

	finally : coda;
}
