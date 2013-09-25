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
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "liblistwise_control.h"

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
static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "w"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "select a window of rows by offset/length"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fail("%s - first argument should be i64", o->op->s);
	if(o->argsl >= 2 && o->args[1]->itype != ITYPE_I64)
		fail("%s - second argument should be i64", o->op->s);

	finally : coda;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int off = 0;
	int len = 0;

	if(o->argsl >= 1)
		off = o->args[0]->i64;
	if(o->argsl >= 2)
		len = o->args[1]->i64;

	if(off < 0)
		off = ls->s[0].l + off;
	if(len == 0)
	{
		if(ls->sel.active == 0)
			len = ls->s[0].l;
		else
			len = ls->sel.active->l;
	}

	int x;
	int i = 0;
	int j = 0;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(i >= off && j < len)
		{
			fatal(lstack_sel_stage, ls, x);
			j++;
		}

		i++;
	}
	LSTACK_ITEREND

	finally : coda;
}
