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

/*

sx  operator - substitution by filename extension
sxf operator - substitution by filename extension (fullmatch)

*/

static int op_validate(operation* o);
static int op_exec_sx(operation*, lwx*, int**, int*, void **);
static int op_exec_sxf(operation*, lwx*, int**, int*, void **);

operator op_desc[] = {
	{
		  .s						= "sx"
		, .optype				= LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_SELECTION_STAGE | LWOP_WINDOWS_STAGE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_sx
		, .desc					= "substitution by filename extension"
	}
	, {
		  .s						= "sxf"
		, .optype				= LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_SELECTION_STAGE | LWOP_WINDOWS_STAGE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_sxf
		, .desc					= "substitution by full filename extension"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && o->argsl != 2)
		failf(LW_ARGSNUM, "expected : 1 or 2, actual : %d", o->argsl);

	finally : coda;
}

static int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, int fullmatch, void ** udata)
{
	char * mxs = 0;
	int mxl = 0;
	char * rxs = 0;
	int rxl = 0;

	if(o->argsl == 1)
	{
		rxs = o->args[0]->s;
		rxl = o->args[0]->l;
	}
	else if(o->argsl == 2)
	{
		rxs = o->args[1]->s;
		rxl = o->args[1]->l;
		mxs = o->args[0]->s;
		mxl = o->args[0]->l;
	}
	
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * s;
		int l;

		// get string representation for the row, ensuring that we operate in tmp space
		fatal(lstack_getstring, ls, 0, x, &s, &l);

		// match starts here
		char * at = 0;

		if(fullmatch)
		{
			// find the entire extension
			char * o = s + l - 1;
			while(o != s && o[0] != '/')
				o--;

			while(o != (s + l) && o[0] != '.')
				o++;

			if(o != (s + l) && o[0] == '.')
			{
				o++;
				if(mxs)
				{
					if((l - (o - s)) == mxl)
					{
						if(memcmp(o, mxs, mxl) == 0)
						{
							// fullmatch on mxs
							at = o;
						}
					}
				}
				else
				{
					// fullmatch
					at = o;
				}
			}
		}
		else if(mxs)
		{
			if(l > mxl)
			{
				if(s[l - mxl - 1] == '.')
				{
					if(memcmp(s + (l - mxl), mxs, mxl) == 0)
					{
						// match on mxs
						at = s + (l - mxl);
					}
				}
			}
		}
		else
		{
			char * o = s + l - 1;
			while(o != s && o[0] != '/' && o[0] != '.')
				o--;

			if(o != s && o[0] == '.')
			{
				// match
				at = o + 1;
			}
		}

		if(at)
		{
			fatal(lstack_writef, ls, 0, x, "%.*s%.*s", at - s, s, rxl, rxs);
			fatal(lstack_selection_stage, ls, x);
			fatal(lstack_windows_stage, ls, x, at - s, rxl);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}

int op_exec_sx(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, ls, ovec, ovec_len, 0, udata);
}

int op_exec_sxf(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, ls, ovec, ovec_len, 1, udata);
}
