/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "internal.h"

#include "parseint.h"
#include "xlinux.h"

/*

cons operator - prepend items to the list

ARGUMENTS

 1+ - use each

OPERATION

*/

static int op_validate(operation*);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "cons"
		, .optype				= LWOP_SELECTION_RESET | LWOP_WINDOWS_RESET | LWOP_ARGS_CANHAVE | LWOP_EMPTYSTACK_YES
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "prepend items to the list"
	}
	, {}
};

static int op_validate(operation* o)
{
	if(o->argsl < 1)
	{
		failf(LW_ARGSNUM, "expected : 1+, actual : %d", o->argsl);
	}

	finally : coda;
}

static int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	for(x = 0; x < o->argsl; x++)
		fatal(lstack_add, ls, o->args[x]->s, o->args[x]->l);

	finally : coda;
}
