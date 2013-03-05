#include <listwise/operator.h>

#include "control.h"

/*

c operator - coalesce lists on the stack

NO ARGUMENTS
 1*. from - merge lists starting at this index, and counting down, into {to} list
            if negative, interpreted as offset from number of lists
              i.e. -1 refers to highest-numbered list index
            default : -1
 2*. to   - default : 0

OPERATION

 1. coalesce all lists onto the top list
 2. clear the current selection

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype					= LWOP_ARGS_CANHAVE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "coalesce lists on the stack"
};

int op_validate(operation* o)
{
	if(o->argsl)
	{
		if(o->args[0]->itype != ITYPE_I64)
			fail("c -- first argument should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int from = -1;

	if(o->argsl)
		from = o->args[0]->i64;

	if(from < 0)
		from = ls->l + from;

	int x;
	for(x = from; x >= 1; x--)
		fatal(lstack_merge, ls, 0, x);

	fatal(lstack_sel_all, ls);

	finally : coda;
}
