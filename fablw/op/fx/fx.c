#include <stdlib.h>
#include <string.h>

#include <listwise.h>
#include <listwise/operator.h>

#include "interpolate.h"

/*

(fab specific) fx operator - set list interpolation mode

ARGUMENTS
	[0] - delimiter string

OPERATION
  with no arguments, fx is equivalent to fxc - set interpolation mode to INTERPOLATION_DELIM_WS
  with an argument, set interpolation mode to INTERPOLATION_DELIM_CUST

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
	if(o->argsl != 0 && o->argsl != 1)
	{
		dprintf(listwise_err_fd, "fx -- arguments : %d", o->argsl);
		return 0;
	}

	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(o->argsl == 0)
	{
		ls->flags = INTERPOLATE_DELIM_WS;
	}
	else if(o->argsl == 1)
	{
		ls->flags = INTERPOLATE_DELIM_CUST;
		free(ls->ptr);
		ls->ptr = strdup(o->args[0]->s);
	}

	return 1;
}
