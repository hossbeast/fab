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
#include <string.h>
#include <alloca.h>

#include "internal.h"

#include "macros.h"

/*

w operator - select window of lines in the top list by offset/length

ARGUMENTS
	 1  - offset to start of window
		    if negative, interpreted as offset from the end of the list
			  default : 0
	*2  - length of window
			  default : 0 - entire list

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "w"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "window"
		, .desc					= "select rows by offset/length"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 1 && (o->argsl % 2) != 0)
		failf(LW_ARGSNUM, "actual : %d", o->argsl);

	int x;
	for(x = 0; x < o->argsl; x++)
	{
		if(o->args[x]->itype != ITYPE_I64)
			failf(LW_ARGSDOM, "should be i64");
	}

	finally : coda;
}

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	// number of already selected rows in top list
	int count = LSTACK_COUNT(lx);

	int i;
	for(i = 0; i < o->argsl; i += 2)
	{
		int win_off = o->args[i]->i64;
		int win_len = 0;

		if(o->argsl > (i + 1))
			win_len = o->args[i + 1]->i64;

		int off = win_off;
		int len = win_len;

		if(win_off < 0)
			off = count + win_off;
		if(win_len == 0)
			len = count - off;
		else
			len = MIN(len, count - off);

		if(off >= 0 && off < count && len > 0)
		{
			int x;
			int j = 0;
			int k = 0;
			LSTACK_ITERATE(lx, x, go)
			if(go)
			{
				if(k >= off && j < len)
				{
					fatal(lstack_selection_stage, lx, x);
					j++;
				}

				k++;
			}
			LSTACK_ITEREND
		}
	}

	finally : coda;
}
