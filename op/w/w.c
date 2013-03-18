#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

w operator - replace selected strings with a window on themselves

ARGUMENTS
	 1  - offset to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0
	*2  - length of window
		    default : 0 - entire string

OPERATION

	1. if nothing selected, select all
	2. foreach selected string
		3. replace that string with a slice of itself specified by the arguments

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_OBJECT_NO
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "replace entries with a window on their contents"
};

int op_validate(operation* o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fail("w - first argument should be i64");
	if(o->argsl >= 2 && o->args[1]->itype != ITYPE_I64)
		fail("w - second argument should be i64");

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
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

		if(go && (!ls->s[0].s || ls->s[0].s[x].type == 0))
		{
			int off = 0;
			int len = 0;

			if(o->argsl >= 1)
				off = o->args[0]->i64;
			if(o->argsl >= 2)
				len = o->args[1]->i64;

			if(off < 0)
				off = ls->s[0].s[x].l + off;
			if(len == 0)
				len = ls->s[0].s[x].l - off;

			if(off < ls->s[0].s[x].l && len > 0)
			{
				// copy of the starting string
				int ssl = ls->s[0].s[x].l;
				char * ss = alloca(ssl + 1);
				memcpy(ss, ls->s[0].s[x].s, ssl);
				ss[ssl] = 0;

				// clear this string on the stack
				lstack_clear(ls, 0, x);

				// write new string using the window
				fatal(lstack_write, ls, 0, x, ss + off, len);

				// record this index was hit
				fatal(lstack_last_set, ls, x);
			}
		}
	}

	finally : coda;
}
