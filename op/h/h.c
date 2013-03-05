#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

h operator - select window of lines in the top list (mnemonic : head)

ARGUMENTS
	 1  - offset to start of window
		    if negative, interpreted as offset from the end of the list
			  default : 0
	*2  - length of window
			  default : 0 - entire list

OPERATION

	1. exchange the top list and the {to} list

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_ARGS_CANHAVE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "select a window of strings from top list"
};

int op_validate(operation* o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fail("h - first argument should be i64");
	if(o->argsl >= 2 && o->args[1]->itype != ITYPE_I64)
		fail("h - second argument should be i64");

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int off = 0;
	int len = 0;

	if(o->argsl >= 1)
		off = o->args[0]->i64;
	if(o->argsl >= 2)
		len = o->args[1]->i64;

	if(off < 0)
		off = ls->s[0].l + off;
	if(len == 0)
	{
		if(ls->sel.all)
			len = ls->s[0].l;
		else
			len = ls->sel.l;
	}

	int x;
	int i = 0;
	int j = 0;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));
		}

		if(go)
		{
			if(i >= off && j < len)
			{
				fatal(lstack_last_set, ls, x);
				j++;
			}

			i++;
		}
	}

	finally : coda;
}
