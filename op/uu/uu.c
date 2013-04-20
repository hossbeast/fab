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
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "control.h"
#include "xstring.h"

#include "parseint.h"

/*

uu operator - select stringwise-unique entries which need not be contiguous

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries which are stringwise NEQ to the preceeding entry

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "select unique (sort not required)"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	size_t num = ls->sel.all ? ls->s[0].l : ls->sel.l;

	// indexes to be sorted
	int * mema = calloc(num, sizeof(*mema));

	char * As = 0;
	int    Asl = 0;
	char * Bs = 0;
	int    Bsl = 0;
	int    r = 0;

	int i = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		mema[i++] = x;
	}
	LSTACK_ITEREND;

	int compar(const void * A, const void * B)
	{
		lstack_string(ls, 0, *(int*)A, &As, &Asl);
		lstack_string(ls, 0, *(int*)B, &Bs, &Bsl);

		return xstrcmp(As, Asl, Bs, Bsl, 0);
	}

	qsort(mema, i, sizeof(*mema), compar);

	if(i)
	{
		fatal(lstack_last_set, ls, mema[0]);
		lstack_string(ls, 0, mema[0], &As, &Asl);

		for(x = 1; x < i; x++)
		{
			if(x % 2)
			{
				lstack_string(ls, 0, mema[x], &Bs, &Bsl);
				r = xstrcmp(As, Asl, Bs, Bsl, 0);
			}
			else
			{
				lstack_string(ls, 0, mema[x], &As, &Asl);
				r = xstrcmp(Bs, Bsl, As, Asl, 0);
			}

			if(r)
			{
				fatal(lstack_last_set, ls, mema[x]);
			}
		}
	}

finally :
	free(mema);
coda;
}
