/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

m operator - match (select by regex)

ARGUMENTS

  0  - regex
	1* - options string

OPERATION

	1. foreach item in selection, or, if no selection, in top list
	2. select that item if regex matches, subject to options

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "m"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "select entries by regex"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl == 1 || o->argsl == 2)
	{
		if(o->args[0]->l == 0)
			fail("m -- empty first argument");

		if(o->argsl == 1 || o->args[1]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl == 2)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[1]->s);
	}
	else
		fail("m -- arguments : %d", o->argsl);

	o->args[0]->itype = 1;

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int x;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))	// could not be selected
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));	// whether it is selected
		}

		if(go)
		{
			char * s = 0;
			int l = 0;
			lstack_string(ls, 0, x, &s, &l);

			fatal(re_exec, &o->args[0]->re, s, l, 0, ovec, ovec_len);

			if((*ovec)[0] > 0)
				fatal(lstack_last_set, ls, x);
		}
	}

	finally : coda;
}
