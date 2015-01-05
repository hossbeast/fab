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

#include "listwise/operator.h"
#include "listwise/re.h"

/*

re operator - execute regex : select by regex, and window regex matches

ARGUMENTS

  0  - regex
	1* - options string

OPERATION

	1. foreach item in selection, or, if no selection, in top list
	2. select that item if regex matches, subject to options

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "re"
		, .optype				= LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "regex"
		, .desc					= "window regex matches"
	}
	, {
		  .s						= "m"
		, .optype				= LWOP_WINDOWS_STAGE | LWOP_SELECTION_ACTIVATE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "match"
		, .desc					= "select by regex"
	}
	, {}
};

static int op_validate(operation* o)
{
	if(o->argsl == 1 || o->argsl == 2)
	{
		if(o->args[0]->l == 0)
			fails(LW_ARGSDOM, "empty argument");

		if(o->argsl == 1 || o->args[1]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl == 2)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[1]->s);
	}
	else
	{
		failf(LW_ARGSNUM, "expected : 1 or 2, actual : %d", o->argsl);
	}

	o->args[0]->itype = ITYPE_RE;

	finally : coda;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int isglobal = o->argsl == 2 && o->args[1]->l && strchr(o->args[1]->s, 'g');

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_getbytes, ls, 0, x, &ss, &ssl);

		fatal(re_execute, &o->args[0]->re, ss, ssl, 0, ovec, ovec_len);

		if((*ovec)[0] > 0)
		{
			do
			{
				fatal(lstack_windows_stage, ls, x, (*ovec)[1], (*ovec)[2] - (*ovec)[1]);

				(*ovec)[0] = 0;
				if(isglobal)
				{
					fatal(re_execute, &o->args[0]->re, ss, ssl, (*ovec)[2], ovec, ovec_len);
				}
			} while((*ovec)[0] > 0);

			// record this index was hit
			fatal(lstack_selection_stage, ls, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
