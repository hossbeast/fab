#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

xm operator - Match by filename eXtension

ARGUMENTS

  [0] - full extension match
  1   - extension

OPERATION

	1. foreach item in selection, or, if no selection, in top list
  2. 
	   2.1 [default mode]    select that item if its stringvalue ends with "." extension
     2.2 [full match mode] select that item if its stringvalue has a complete extension equal to extension

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_ARGS_CANHAVE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "select by filename extension"
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && o->argsl != 2)
		fail("xm -- arguments : %d", o->argsl);

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	char* xs;
	int xl;

	int fullmatch = 0;
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
