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
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "xlinux.h"
#include "parseint.h"
#include "strutil.h"

/*

uu operator - select stringwise-unique entries which need not be contiguous

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries which are stringwise NEQ to the preceeding entry

*/

static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
			.s						= "uu"
		, .optype				= LWOP_SELECTION_ACTIVATE
		, .op_exec			= op_exec
		, .desc					= "select unique - sort not required"
	}
	, {}
};

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	enter;

	// indexes to be sorted
	int * mema = 0;

	if(!(lx->sel.active && lx->sel.active->lease == lx->sel.active_era && lx->sel.active->state == LWX_SELECTION_NONE))
	{
		char * As = 0;
		int    Asl = 0;
		char * Bs = 0;
		int    Bsl = 0;
		int    r = 0;

		fatal(xmalloc, &mema, (lx->sel.active ? lx->sel.active->l : lx->s[0].l) * sizeof(*mema));

		int i = 0;
		int x;
		LSTACK_ITERATE(lx, x, go)
		if(go)
		{
			mema[i++] = x;
		}
		LSTACK_ITEREND;

		xapi compar(const void * A, const void * B, void * T, int * r)
		{
      enter;

			fatal(lstack_getbytes, lx, 0, *(int*)A, &As, &Asl);
			fatal(lstack_getbytes, lx, 0, *(int*)B, &Bs, &Bsl);

			(*r) = estrcmp(As, Asl, Bs, Bsl, 0);

			finally : coda;
		}

		fatal(xqsort_r, mema, i, sizeof(*mema), compar, 0);

		if(i)
		{
			fatal(lstack_selection_stage, lx, mema[0]);
			fatal(lstack_getbytes, lx, 0, mema[0], &As, &Asl);

			for(x = 1; x < i; x++)
			{
				if(x % 2)
				{
					fatal(lstack_getbytes, lx, 0, mema[x], &Bs, &Bsl);
					r = estrcmp(As, Asl, Bs, Bsl, 0);
				}
				else
				{
					fatal(lstack_getbytes, lx, 0, mema[x], &As, &Asl);
					r = estrcmp(Bs, Bsl, As, Asl, 0);
				}

				if(r)
				{
					fatal(lstack_selection_stage, lx, mema[x]);
				}
			}
		}
	}

finally :
	free(mema);
coda;
}
