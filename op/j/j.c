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
#include <listwise/lstack.h>

#include "control.h"

/*

j operator - join strings

ARGUMENTS
	[1] - delimiter, default : none

OPERATION

	1. if nothing selected, select all
	2. join selected strings using the delimiter
	3. selection is now the new entry

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_OBJECT_NO
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "join strings possibly with a delimiter"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	// delimiter string
	char * ds = 0;
	int dl = 0;
	if(o->argsl)
	{
		ds = o->args[0]->s;
		dl = o->args[0]->l;
	}

	// new entry accumulated here
	char * ns = 0;
	int nl = 0;
	int na = 0;

	// count entries that will be deleted
	int cnt = 0;
	int x;
	LSTACK_ITERREV(ls, x, go);
	if(go && ls->s[0].s[x].type == 0)
	{
		cnt++;
	}
	LSTACK_ITEREND;

	// deleted entries stored here
	typeof(ls->s[0].s[0]) Ts[cnt];
	typeof(ls->s[0].t[0]) Tt[cnt];

	int i = 0;
	LSTACK_ITERREV(ls, x, go);
	if(go && ls->s[0].s[x].type == 0)
	{
		// reallocate accumulator if necessary
		if((nl + dl + ls->s[0].s[x].l) >= na)
		{
			int sz = na ?: 10;
			while(sz <= (nl + dl + ls->s[0].s[x].l))
				sz = sz * 2 + sz / 2;

			char * tmp = ns;
			ns = alloca(sz);
			memcpy(ns, tmp, nl);
			na = sz;
		}

		// prepend delimiter
		if(i)
		{
			memmove(ns + dl, ns, nl);
			memcpy(ns, ds, dl);
			nl += dl;
		}

		// prepend this entry to accumulator
		memmove(ns + ls->s[0].s[x].l, ns, nl);
		memcpy(ns, ls->s[0].s[x].s, ls->s[0].s[x].l);
		nl += ls->s[0].s[x].l;

		// store strings about to be overwritten
		Ts[i] = ls->s[0].s[x];
		Tt[i] = ls->s[0].t[x];
		i++;

		// overwrite strings just used
		memmove(
			  &ls->s[0].s[x]
			, &ls->s[0].s[x+1]
			, (ls->s[0].l - x - 1) * sizeof(ls->s[0].s[0])
		);
		memmove(
			  &ls->s[0].t[x]
			, &ls->s[0].t[x+1]
			, (ls->s[0].l - x - 1) * sizeof(ls->s[0].t[0])
		);
	}
	LSTACK_ITEREND;

	// shrink
	ls->s[0].l -= i;

	// place deleted entries at the end to be reused/freed by the library
	memcpy(&ls->s[0].s[ls->s[0].l], Ts, sizeof(Ts));
	memcpy(&ls->s[0].t[ls->s[0].l], Tt, sizeof(Tt));

	// add new entry from the accumulator
	fatal(lstack_add, ls, ns, nl);

	// selection
	fatal(lstack_sel_all, ls);
	fatal(lstack_last_set, ls, ls->s[0].l - 1);

	finally : coda;
}





