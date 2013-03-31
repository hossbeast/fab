/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

xs operator - eXtension Substitution

ARGUMENTS
	[0] - full extension match
	[1] - matching extension string
	 2  - replacement extension string

OPERATION

	1. if nothing selected, select all
	2. if a matching extension string is given
	  2.1 [default mode]    select that item if its stringvalue ends with "." extension
	  2.2 [full match mode] select that item if its stringvalue has a complete extension equal to extension
	3. foreach selected string
		3.1. [default mode]    replace the matched portion, or the final extension component of the string with '.' extension
		3.1. [full match mode] replace the matched portion, or the complete extension of the string with '.' extension

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_OBJECT_NO
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "substitution by filename extension"
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && o->argsl != 2 && o->argsl != 3)
		fail("xs -- arguments : %d", o->argsl);
	if(o->argsl == 1 && o->args[0]->itype == ITYPE_I64)
		fail("xs -- arguments : %d, [0]=ITYPE_I64", o->argsl);

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int fullmatch = 0;
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
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));
		}

		if(go && ls->s[0].s[x].type == 0 && ls->s[0].s[x].l)
		{
			char * s = ls->s[0].s[x].s;
			int l = ls->s[0].s[x].l;

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
				int o = at - s;
				int newl = o + rxl;

				fatal(lstack_ensure, ls, 0, x, newl);
				sprintf(ls->s[0].s[x].s + o, "%.*s", rxl, rxs);
				ls->s[0].s[x].l = newl;
				ls->s[0].t[x].w = 0;

				fatal(lstack_last_set, ls, x);
			}
		}
	}

	finally : coda;
}





