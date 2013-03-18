#include <stdlib.h>
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "control.h"

/*

v operator - invert current selection

NO ARGUMENTS

OPERATION

 1. invert selectedness of each item

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "invert selection"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(ls->l)
	{
		int x;
		LSTACK_ITERATE(ls, x, go);
		if(!go)
		{
			fatal(lstack_last_set, ls, x);
		}
		LSTACK_ITEREND;
	}

	finally : coda;
}
