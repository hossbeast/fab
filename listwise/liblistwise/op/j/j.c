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

#include "xlinux.h"

#include "listwise/operator.h"
#include "listwise/lwx.h"

/*

j operator - join rows

ARGUMENTS
	[1] - delimiter, default : none

OPERATION

	1. if nothing selected, select all
	2. join selected rows using the delimiter
	3. selection is now the new entry

*/

static xapi op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "j"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .mnemonic			= "join"
		, .desc					= "concatenate rows possibly with a delimiter"
	}
	, {}
};

xapi op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
  enter;

	typeof(*ls->s[0].s) * Ts = 0;
	typeof(*ls->s[0].t) * Tt = 0;

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
	if(go)
	{
		cnt++;
	}
	LSTACK_ITEREND;

	// deleted entries stored here
  fatal(xmalloc, &Ts, sizeof(*Ts) * cnt);
  fatal(xmalloc, &Tt, sizeof(*Tt) * cnt);

	int i = 0;
	LSTACK_ITERREV(ls, x, go);
	if(go)
	{
		char * s;
		int l;
		fatal(lstack_getbytes, ls, 0, x, &s, &l);

		// reallocate accumulator if necessary
		if((nl + dl + l) >= na)
		{
			int sz = na ?: 10;
			while(sz <= (nl + dl + l))
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
		memmove(ns + l, ns, nl);
		memcpy(ns, s, l);
		nl += l;

		// store rows about to be overwritten
		Ts[i] = ls->s[0].s[x];
		Tt[i] = ls->s[0].t[x];
		i++;

		// overwrite rows just used
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
	memcpy(&ls->s[0].s[ls->s[0].l], Ts, sizeof(ls->s[0].s[0]));
	memcpy(&ls->s[0].t[ls->s[0].l], Tt, sizeof(ls->s[0].t[0]));

	// add new entry from the accumulator
	fatal(lstack_addw, ls, ns, nl);

	// selection
	fatal(lstack_selection_stage, ls, ls->s[0].l - 1);

	finally:
    free(Ts);
    free(Tt);
  coda;
}
