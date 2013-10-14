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
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "liblistwise_control.h"

#include "xstring.h"
#include "xmem.h"
#include "parseint.h"

/*

sn operator  - sort numbers (in ascending order)
ss operator  - sort strings (in ascending order)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. sort the items amongst themselves

*/

static int op_exec_ss(operation*, lwx*, int**, int*);
static int op_exec_sn(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "ss"
		, .optype				= LWOP_MODIFIERS_CANHAVE
		, .op_exec			= op_exec_ss
		, .desc					= "sort stringwise"
	}
	, {
		  .s						= "sn"
		, .optype				= 0
		, .op_exec			= op_exec_sn
		, .desc					= "sort numeric"
	}
};

#define STRING_WCASE	1
#define STRING_NCASE	2
#define NUMERIC				3

static int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, int mode)
{
	int * mema = 0;
	int * memb = 0;

	size_t num = lx->s[0].l;
	if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era)
		num = lx->sel.active->l;

	// indexes to be sorted
	fatal(xmalloc, &mema, num * sizeof(*mema));

	// copy of indexes
	fatal(xmalloc, &memb, num * sizeof(*memb));

	int i = 0;
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		mema[i] = x;
		memb[i] = x;
		i++;
	}
	LSTACK_ITEREND;

	int waserr = 0;
	int compar(const void * A, const void * B)
	{
		if(waserr)
			return 0;

		char *	As = 0;
		int			Asl = 0;
		char *	Bs = 0;
		int			Bsl = 0;

		int r = 0;

#define FAIL(s)		do { if(s) { dprintf(listwise_error_fd, s); } waserr = 1; return 0; } while(0)

		if(mode == NUMERIC)
		{
			if((As = lstack_string(lx, 0, *(int*)A)) == 0)
				FAIL("allocation failure");
			if((Bs = lstack_string(lx, 0, *(int*)B)) == 0)
				FAIL("allocation failure");
		}
		else
		{
			if(lstack_getstring(lx, 0, *(int*)A, &As, &Asl))
				FAIL("allocation failure");
			if(lstack_getstring(lx, 0, *(int*)B, &Bs, &Bsl))
				FAIL("allocation failure");
		}

		if(mode == NUMERIC)
		{
			intmax_t Aval;
			intmax_t Bval;

			if(parseint(As, SCNdMAX, INTMAX_MIN, INTMAX_MAX, 0, 0xFF, &Aval, 0) == 0)
			{
				if(parseint(Bs, SCNdMAX, INTMAX_MIN, INTMAX_MAX, 0, 0xFF, &Bval, 0) == 0)
				{
					r = Aval - Bval;
				}
			}
		}
		else
		{
			r = xstrcmp(As, Asl, Bs, Bsl, mode == STRING_NCASE);
		}

		return r;
	}

	qsort(mema, i, sizeof(*mema), compar);
	if(waserr)
		qfail();

	for(x = 0; x < i; x++)
	{
		int a = mema[x];
		int b = memb[x];

		mema[x] = 0xFFFF;

		if(a != b)
		{
			int y;
			for(y = 0; y < i; y++)
			{
				if(mema[y] == b)
					break;
			}

			// swap
			typeof(lx->s[0].s[0]) Ts = lx->s[0].s[a];
			typeof(lx->s[0].t[0]) Tt = lx->s[0].t[a];
			typeof(lx->win.s[0])  Tw = lx->win.s[a];

			lx->s[0].s[a] = lx->s[0].s[b];
			lx->s[0].t[a] = lx->s[0].t[b];
			lx->win.s[a]  = lx->win.s[b];

			lx->s[0].s[b] = Ts;
			lx->s[0].t[b] = Tt;
			lx->win.s[b]  = Tw;

			if(y < i)
				mema[y] = a;
		}
	}

finally:
	free(mema);
	free(memb);
coda;
}

int op_exec_ss(operation * o, lwx * lx, int ** ovec, int * ovec_len)
{
	int ncase = 0;

	if(o->argsl >= 1)
		ncase = strchr(o->args[0]->s, 'i') != 0;

	return op_exec(o, lx, ovec, ovec_len, ncase ? STRING_NCASE : STRING_WCASE);
}

int op_exec_sn(operation * o, lwx * lx, int ** ovec, int * ovec_len)
{
	return op_exec(o, lx, ovec, ovec_len, NUMERIC);
}
