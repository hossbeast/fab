/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

static xapi op_validate(operation* o);
static xapi op_exec_lx(operation*, lwx*, int**, int*);
static xapi op_exec_lxf(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "lx"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_STAGE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_lx
		, .mnemonic			= "locate-extension"
		, .desc					= "select by filename extension"
	}
	, {
		  .s						= "lxf"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_STAGE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_lxf
		, .mnemonic			= "locate-extension-full"
		, .desc					= "select by full filename extension"
	}
	, {}
};

xapi op_validate(operation* o)
{
  enter;

	if(o->argsl < 1)
		failf(LISTWISE_ARGSNUM, "expected %s", "actual %d", "1 or more", o->argsl);

	finally : coda;
}

static xapi op_exec(operation* op, lwx* ls, int** ovec, int* ovec_len, int fullmatch)
{
  enter;

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * s = 0;
		int l = 0;
		fatal(lstack_getbytes, ls, 0, x, &s, &l);

		if(fullmatch)
		{
			// find the entire extension
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

					int i;
					for(i = 0; i < op->argsl; i++)
					{
						if((l - (o - s)) != op->args[i]->l)
							continue;
						if(memcmp(o, op->args[i]->s, op->args[i]->l) == 0)
							break;
					}

					if(i < op->argsl)	// match
					{
						fatal(lstack_windows_stage, ls, x, o - s, op->args[i]->l);
						fatal(lstack_selection_stage, ls, x);
					}
				}
			}
		}
		else
		{
			// does stringvalue terminate with .<extension>
			int i;
			for(i = 0; i < op->argsl; i++)
			{
				if((l - op->args[i]->l) < 1)
					continue;
				if(s[l - op->args[i]->l - 1] != '.')
					continue;
				if(memcmp(s + (l - op->args[i]->l), op->args[i]->s, op->args[i]->l) == 0)
					break;
			}

			if(i < op->argsl)
			{
				fatal(lstack_windows_stage, ls, x, l - op->args[i]->l, op->args[i]->l);
				fatal(lstack_selection_stage, ls, x);
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}

xapi op_exec_lx(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	xproxy(op_exec, o, ls, ovec, ovec_len, 0);
}

xapi op_exec_lxf(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	xproxy(op_exec, o, ls, ovec, ovec_len, 1);
}
