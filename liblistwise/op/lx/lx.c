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

#include "listwise/internal.h"

/*

lx operator  - Locate by filename eXtension
lxf operator - Locate by Full filename eXtension

ARGUMENTS

  [0] - full extension match
  1   - extension

OPERATION

	1. foreach item in selection, or, if no selection, in top list
  2. 
	   2.1 [default mode]    select that item if its stringvalue ends with "." extension
     2.2 [full match mode] select that item if its stringvalue has a complete extension equal to extension

lxf - exactly as the lx operator, except fullmatch mode is the default operation

*/

static int op_validate(operation* o);
static int op_exec_lx(operation*, lwx*, int**, int*);
static int op_exec_lxf(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "lx"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_STAGE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_lx
		, .mnemonic			= "locate extension"
		, .desc					= "select by filename extension"
	}
	, {
		  .s						= "lxf"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_STAGE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_lxf
		, .mnemonic			= "locate extension full"
		, .desc					= "select by full filename extension"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && o->argsl != 2)
		fail(LW_ARGSNUM, "expected : 1 or 2, actual : %d", o->argsl);

	finally : coda;
}

static int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, int fullmatch)
{
	char* xs;
	int xl;

	if(o->args[0]->itype == ITYPE_I64)
	{
		fullmatch = o->args[0]->i64;

		xs = o->args[1]->s;
		xl = o->args[1]->l;
	}
	else
	{
		xs = o->args[0]->s;
		xl = o->args[0]->l;
	}

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * s = 0;
		int l = 0;
		fatal(lstack_getbytes, ls, 0, x, &s, &l);

		if(fullmatch)
		{
			if(l > xl)
			{
				// find the entire extension, is it exactly equal to <extension>
				char * o = s + l - 1;
				while(o != s && o[0] != '/')
					o--;

				while(o != (s + l) && o[0] != '.')
					o++;

				if(o[0] == '.')
				{
					if(o != (s + l))
					{
						o++;
						if((l - (o - s)) == xl)
						{
							if(memcmp(o, xs, xl) == 0)
							{
								fatal(lstack_window_stage, ls, x, o - s, xl);
								fatal(lstack_sel_stage, ls, x);
							}
						}
					}
				}
			}
		}
		else
		{
			if(l > xl)
			{
				// does stringvalue terminate with .<extension>
				if(s[l - xl - 1] == '.')
				{
					if(memcmp(s + (l - xl), xs, xl) == 0)
					{
						fatal(lstack_window_stage, ls, x, l - xl, xl);
						fatal(lstack_sel_stage, ls, x);
					}
				}
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}

static int op_exec_lx(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 0);
}

static int op_exec_lxf(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 1);
}
