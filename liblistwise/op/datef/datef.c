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
#include <string.h>
#include <time.h>

#include <listwise.h>
#include <listwise/operator.h>

#include "control.h"
#include "parseint.h"

/*

datef operator - format unix date to a string

ARGUMENTS

  0* - date format string

OPERATION

	1. foreach item in selection, or, if no selection, in top list
	2. apply date format and replace entry

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "datef"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_OBJECT_NO
		, .op_exec			= op_exec
		, .desc					= "format unix date to a string"
	}, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	char space[16];
	char space2[64];

	char * fmt = "%a %b %d %Y %H:%M:%S";
	if(o->argsl)
		fmt = o->args[0]->s;

	int x;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			go = 0;
			if(ls->sel.sl > (x/8))
			{
				go = ls->sel.s[x/8] & (0x01 << (x%8));
			}
		}

		if(go && ls->s[0].s[x].type == 0)
		{
			// copy of the starting string
			snprintf(space, sizeof(space), "%.*s", ls->s[0].s[x].l, ls->s[0].s[x].s);

			time_t T;
			if(parseuint(space, SCNu32, 0, UINT32_MAX, 1, 10, &T, 0) == 0)
			{
				struct tm tm;
				strftime(space2, sizeof(space2), fmt, localtime_r(&T, &tm));

				// clear this string on the stack
				lstack_clear(ls, 0, x);

				// text in the subject string before the first match
				fatal(lstack_append
					, ls
					, 0
					, x
					, space2
					, strlen(space2)
				);

				fatal(lstack_last_set, ls, x);
			}
		}
	}

	finally : coda;
}
