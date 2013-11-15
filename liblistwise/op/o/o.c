/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

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

operator op_desc[] = {
	{
		  .s						= "o"
		, .optype				= 0
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "OR : aggregate selections across operators"
	}, {}
};

int op_validate(operation* o)
{
	return 0;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return 0;
}
