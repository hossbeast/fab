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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

/*

xsf operator - Fullmatch eXtension Substitution

exactly as the xs operator, except fullmatch mode is the default

*/

static int op_validate(operation* o);
static int op_exec_xs(operation*, lstack*, int**, int*);
static int op_exec_xsf(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "xs"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_xs
		, .desc					= "substitution by filename extension"
	}
	, {
		  .s						= "xsf"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_validate	= op_validate
		, .op_exec			= op_exec_xsf
		, .desc					= "substitution by full filename extension"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && o->argsl != 2 && o->argsl != 3)
		fail("xsf -- arguments : %d", o->argsl);
	if(o->argsl == 1 && o->args[0]->itype == ITYPE_I64)
		fail("xsf -- arguments : %d, [0]=ITYPE_I64", o->argsl);

	finally : coda;
}

static int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len, int fullmatch)
{
	char * mxs = 0;
	int mxl = 0;
	char * rxs = 0;
	int rxl = 0;

	if(o->args[0]->itype == ITYPE_I64)
	{
		fullmatch = o->args[0]->i64;

		if(o->argsl == 2)
		{
			rxs = o->args[1]->s;
			rxl = o->args[1]->l;
		}
		else if(o->argsl == 3)
		{
			rxs = o->args[2]->s;
			rxl = o->args[2]->l;
			mxs = o->args[1]->s;
			mxl = o->args[1]->l;
		}
	}
	else if(o->argsl == 1)
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
		fatal(lstack_getbytes, ls, 0, x, &s, &l);

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
			// calling getstring ensures that s points to the temp space for this row
			// if getbytes prevously returned the temp space, then this call resolves as a no-op
			fatal(lstack_getstring, ls, 0, x, &s, &l);

			fatal(lstack_writef, ls, 0, x, "%.*s%.*s", at - s, s, rxl, rxs);
			fatal(lstack_last_set, ls, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}

int op_exec_xs(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 0);
}

int op_exec_xsf(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 1);
}
