/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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
#include <string.h>
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

/*

ln operator - line numbers : insert <num> at the beginning of each entry

*/

static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "ln"
		, .optype				= LWOP_SELECTION_RESET | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .mnemonic			= "line numbers"
		, .desc					= "prepend line number to selected entries"
	}
	, {}
};

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_getstring, lx, 0, x, &ss, &ssl);
		fatal(lstack_writef, lx, 0, x, "%d ", x);
		fatal(lstack_catw, lx, 0, x, ss, ssl);
	}
	LSTACK_ITEREND

	finally : coda;
}
