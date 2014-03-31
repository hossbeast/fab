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
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <inttypes.h>

#include "internal.h"

/*

A operator - select leading context
B operator - select trailing context
C operator - select context

NO ARGUMENTS

OPERATION

 1. select items following last selected item

*/

static int op_validate(operation* o);
static int op_exec_A(operation*, lwx*, int**, int*, void**);
static int op_exec_B(operation*, lwx*, int**, int*, void**);
static int op_exec_C(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "A"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_A
		, .desc					= "select leading context"
	}
	, {
		  .s						= "B"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_B
		, .desc					= "select trailing context"
	}
	, {
		  .s						= "C"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_C
		, .desc					= "select leading and trailing context"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1)
	{
		failf(LW_ARGSNUM, "expected 1 actual : %d", o->argsl);
	}

	if(o->args[0]->itype != ITYPE_I64)
	{
		failf(LW_ARGSTYPE, "should be i64");
	}

	if(o->args[0]->i64 < 1)
	{
		failf(LW_ARGSDOM, "expected >= 0, actual : %"PRIi64, o->args[0]->i64);
	}

	finally : coda;
}

static int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, int bef, int af, void ** udata)
{
	int C = o->args[0]->i64;

	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		int i;
		for(i = 0; i < C; i++)
		{
			if(bef)
			{
				if((x - i - 1) >= 0)
					fatal(lstack_selection_stage, lx, x - i - 1);
			}
			if(af)
			{
				if((x + i + 1) < lwx_rows(lx, 0))
					fatal(lstack_selection_stage, lx, x + i + 1);
			}
			fatal(lstack_selection_stage, lx, x);
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}

int op_exec_A(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, lx, ovec, ovec_len, 0, 1, udata);
}

int op_exec_B(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, lx, ovec, ovec_len, 1, 0, udata);
}

int op_exec_C(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, lx, ovec, ovec_len, 1, 1, udata);
}
