#include <stdlib.h>
#include <string.h>

#include <listwise.h>
#include <listwise/operator.h>

#include "interpolate.h"

/*

(fab specific) fxw operator - set list interpolation mode

NO ARGUMENTS

OPERATION
  set list interpolation mode to INTERPOLATION_DELIM_WS

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_ARGS_CANHAVE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "(fab specific) list interpolation mode"
};

int op_validate(operation* o)
{
	if(o->argsl != 0)
	{
		dprintf(listwise_err_fd, "fxw -- arguments : %d", o->argsl);
		return 0;
	}

	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	ls->flags = INTERPOLATE_ADJOIN;

	return 1;
}
