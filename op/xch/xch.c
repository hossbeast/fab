#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

xch operator - eXCHange lists

ARGUMENTS
 1*. to - exchange top list with list at this index
            if negative, interpreted as offset from number of lists
              i.e. -1 refers to highest-numbered list index
            default : 1

OPERATION

	1. exchange the top list and the {to} list

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "exchange lists"
};

int op_validate(operation* o)
{
	if(o->argsl)
	{
		if(o->args[0]->itype != ITYPE_I64)
			fail("x -- first argument should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int to = 1;
	if(o->argsl)
		to = o->args[0]->i64;
	if(to < 0)
		to = ls->l + to;

	if(!(ls->l > to))
	{
		dprintf(listwise_err_fd, "xch/%d out of range\n", to);
	}
	else
	{
		typeof(ls->s[0]) T = ls->s[0];
		ls->s[0] = ls->s[to];
		ls->s[to] = T;
	}

	fatal(lstack_sel_all, ls);

	finally : coda;
}
