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
#include <time.h>

#include <listwise.h>
#include <listwise/operator.h>

#include "listwise/operator.h"

#include "parseint.h"

/*

datef operator - format unix date to a string

ARGUMENTS

  0* - date format string

OPERATION

	1. foreach item in selection, or, if no selection, in top list
	2. apply date format and replace entry

*/

static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "datef"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .desc					= "format unix date to a string"
		, .mnemonic			= "date format"
	}, {}
};

int op_exec(operation* o, lwx * lx, int** ovec, int* ovec_len, void ** udata)
{
	char space[64];

	char * fmt = "%a %b %d %Y %H:%M:%S";
	if(o->argsl)
		fmt = o->args[0]->s;

	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_getstring, lx, 0, x, &ss, &ssl);

		// parse as time_t
		time_t T;
		if(parseuint(ss, SCNu32, 0, UINT32_MAX, 1, 10, &T, 0) == 0)
		{
			// format to string
			struct tm tm;
			strftime(space, sizeof(space), fmt, localtime_r(&T, &tm));

			// rewrite the row
			fatal(lstack_writes, lx, 0, x, space);

			// add to staged selections
			fatal(lstack_selection_stage, lx, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
