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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

#include "xstring.h"
#include "xmem.h"

/*

ss operator  - sort strings (in ascending order)
ssm operator - sort strings (in ascending order) on a regex
ssw operator - sort strings (in ascending order) on a window

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. sort the items amongst themselves

*/

static int op_validate_ss(operation* o);
static int op_validate_ssm(operation* o);
static int op_validate_ssw(operation* o);
static int op_exec_ss(operation*, lstack*, int**, int*);
static int op_exec_ssm(operation*, lstack*, int**, int*);
static int op_exec_ssw(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "ss"
		, .optype				= LWOP_SELECTION_READ
		, .op_validate	= op_validate_ss
		, .op_exec			= op_exec_ss
		, .desc					= "sort stringwise"
	}
	, {
		  .s						= "ssm"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_ssm
		, .op_exec			= op_exec_ssm
		, .desc					= "sort stringwise on regex"
	}
	, {
		  .s						= "ssw"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_ssw
		, .op_exec			= op_exec_ssw
		, .desc					= "sort stringwise on window"
	}, {}
};

int op_validate_ss(operation * o)
{
	return 0;
}

int op_validate_ssm(operation * o)
{
	if(o->argsl == 1 || o->argsl == 2)
	{
		if(o->args[0]->l == 0)
			fail("ssm -- empty first argument");

		if(o->argsl == 1 || o->args[1]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl == 2)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[1]->s);
	}
	else
		fail("ssm -- arguments : %d", o->argsl);

	o->args[0]->itype = 1;

	finally : coda;
}

int op_validate_ssw(operation * o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fail("ssw - first argument should be i64");
	if(o->argsl >= 2 && o->args[1]->itype != ITYPE_I64)
		fail("ssw - second argument should be i64");

	finally : coda;
}

static int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len, int mode)
{
	int win_off = 0;
	int win_len = 0;

	if(mode == 2)
	{
		if(o->argsl >= 1)
			win_off = o->args[0]->i64;
		if(o->argsl >= 2)
			win_len = o->args[1]->i64;
	}

printf("win_off : %d\n", win_off);
printf("win_len : %d\n", win_len);

	size_t num = ls->sel.all ? ls->s[0].l : ls->sel.l;

	// indexes to be sorted
	int * mema = 0;
	fatal(xmalloc, &mema, num * sizeof(*mema));

	// copy of indexes
	int * memb = 0;
	fatal(xmalloc, &memb, num * sizeof(*memb));

	// copies of entries to be swapped
	typeof(ls->s[0].s[0]) * T = 0;
	fatal(xmalloc, &T, ls->s[0].l * sizeof(*T));
	memcpy(T, ls->s[0].s, ls->s[0].l * sizeof(*T));

	int i = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		mema[i] = x;
		memb[i] = x;
		i++;
	}
	LSTACK_ITEREND;

	int qfail = 0;
	int compar(const void * A, const void * B)
	{
		if(qfail)
			return 0;

		char *	As = 0;
		int			Asl = 0;
		char *	Bs = 0;
		int			Bsl = 0;

		if(lstack_string(ls, 0, *(int*)A, &As, &Asl))
			{ qfail = 1; return 0; }
		if(lstack_string(ls, 0, *(int*)B, &Bs, &Bsl))
			{ qfail = 1; return 0; }

		if(mode == 1)
		{
			if(re_exec(&o->args[0]->re, As, Asl, 0, ovec, ovec_len))
				{ qfail = 1; return 0; }
			if((*ovec)[0] <= 0)
			{
				As = 0;
			}
			else
			{
				As = As + (*ovec)[1];
				Asl = (*ovec)[2] - (*ovec)[1];
			}

			if(re_exec(&o->args[0]->re, Bs, Bsl, 0, ovec, ovec_len))
				{ qfail = 1; return 0; }
			if((*ovec)[0] <= 0)
			{
				Bs = 0;
			}
			else
			{
				Bs = Bs + (*ovec)[1];
				Bsl = (*ovec)[2] - (*ovec)[1];
			}
		}
		else if(mode == 2)
		{
			int off = win_off;
			int len = win_len;

			if(win_off < 0)
				off = Asl + win_off;
			if(win_len == 0)
				len = Asl - win_off;

			if(off >= Asl || len <= 0)
			{
				As = 0;
			}
			else
			{
				As += win_off;
				Asl = win_len;
			}

printf("A off : %d\n", off);
printf("A len : %d\n", len);
printf("A     : %.*s\n", Asl, As);

			off = win_off;
			len = win_len;

			if(win_off < 0)
				off = Bsl + win_off;
			if(win_len == 0)
				len = Bsl - win_off;

			if(off >= Asl || len <= 0)
			{
				Bs = 0;
			}
			else
			{
				Bs += win_off;
				Bsl = win_len;
			}

//printf("B off : %d\n", off);
//printf("B len : %d\n", len);
//printf("B     : %.*s\n", Bsl, Bs);
		}

		if(As && Bs)
			return xstrcmp(As, Asl, Bs, Bsl, 0);
		else if(As)
			return 1;
		else if(Bs)
			return -1;

		return 0;
	}

	qsort(mema, i, sizeof(*mema), compar);

	for(x = 0; x < i; x++)
	{
		ls->s[0].s[memb[x]] = T[mema[x]];
	}

	free(mema);
	free(memb);
	free(T);

	finally : coda;
}

int op_exec_ss(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 0);
}

int op_exec_ssm(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 1);
}

int op_exec_ssw(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 2);
}
