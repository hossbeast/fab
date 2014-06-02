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

#include "internal.h"

#include "xlinux.h"

/*

cp operator - duplicate list entries

ARGUMENTS

 number of copies - default : 1

OPERATION

 use current select, ELSE
 use entire top list

 for each entry
	1. create N copies of this entry at indexes x + 1 .. N

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "cp"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_ARGS_CANHAVE | LWOP_WINDOWS_RESET
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "duplicate list entries"
		, .mnemonic			= "copy"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fails(LW_ARGSDOM, "should be i64");

	finally : coda;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int N = 1;
	if(o->argsl)
		N = o->args[0]->i64;

	int c = 0;
	int k = ls->s[0].l;

	int x;
	LSTACK_ITERREV(ls, x, go);
	if(go)
	{
		// prepare N entries at x+1 for writing
		fatal(lstack_displace, ls, 0, x + 1, N);
		
		int y;
		for(y = 1; y <= N; y++)
		{
			if(ls->s[0].s[x].type)
			{
				fatal(lstack_obj_alt_write, ls, 0, x+y, *(void**)ls->s[0].s[x].s, ls->s[0].s[x].type);
			}
			else
			{
				fatal(lstack_alt_writew, ls, 0, x+y, ls->s[0].s[x].s, ls->s[0].s[x].l);
			}
		}
	}
	LSTACK_ITEREND;

	LSTACK_ITERATE(ls, x, go);
	if(x < k)
	{
		if(go)
		{
			int y;
			for(y = 0; y <= N; y++)
				fatal(lstack_selection_stage, ls, x + y + c);

			c += N;
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}
