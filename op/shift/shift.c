#include <listwise/operator.h>

#include "control.h"

/*

shift operator - delete the 0th list from the stack

ARGUMENTS
	[1] - number of times to shift, default : 1

OPERATION

 N times (0 means until only the Nth list remains)
	 1. delete the 0th list from the stack

*/

static int op_exec(operation*, lstack*, int**, int*);
static int op_validate(operation*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE
	, .op_exec			= op_exec
	, .op_validate	= op_validate
	, .desc					= "delete the 0th list"
};

int op_validate(operation* o)
{
	if(o->argsl && o->args[0]->itype != ITYPE_I64)
		fail("shift -- expected first argument : i64");

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int N = 1;
	if(o->argsl)
		N = o->args[0]->i64;
	if(N == 0)
		N = ls->l - 1;

	int x;
	for(x = 0; x < N; x++)
		fatal(lstack_shift, ls);

	// if anything was selected, its now used up
	fatal(lstack_sel_all, ls);

	finally : coda;
}
