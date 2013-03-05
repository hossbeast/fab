#include <stdlib.h>
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "control.h"

/*

o operator - aggregate selections (this OR that)

NO ARGUMENTS

OPERATION

	1. do not reset the "last list" before the next operator
	2. do not excute the implicit "y" after the preceeding operator

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= 0
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "OR : aggregate selections across operators"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return 1;
}
