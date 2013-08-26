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

#include "liblistwise_control.h"

/*

xmf operator - Match by Full filename eXtension

exactly as the xm operator, except fullmatch mode is the default operation

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "xmf"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "select by full filename extension"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && o->argsl != 2)
		fail("xmf -- arguments : %d", o->argsl);

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	char* xs;
	int xl;

	int fullmatch = 1;
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
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))	// could not be selected
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));	// whether it is selected
		}

		if(go)
		{
			char * s = 0;
			int l = 0;
			lstack_string(ls, 0, x, &s, &l);

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
									fatal(lstack_last_set, ls, x);
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
							fatal(lstack_last_set, ls, x);
						}
					}
				}
			}
		}
	}

	finally : coda;
}
