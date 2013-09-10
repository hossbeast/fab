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

static int op_validate_ssm(operation* o);
static int op_validate_ssw(operation* o);
static int op_validate_snm(operation* o);
static int op_validate_snw(operation* o);
static int op_exec_ss(operation*, lstack*, int**, int*);
static int op_exec_ssm(operation*, lstack*, int**, int*);
static int op_exec_ssw(operation*, lstack*, int**, int*);
static int op_exec_sn(operation*, lstack*, int**, int*);
static int op_exec_snm(operation*, lstack*, int**, int*);
static int op_exec_snw(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "ss"
		, .optype				= LWOP_SELECTION_READ | LWOP_MODIFIERS_CANHAVE
		, .op_exec			= op_exec_ss
		, .desc					= "sort stringwise"
	}
	, {
		  .s						= "sn"
		, .optype				= LWOP_SELECTION_READ
		, .op_exec			= op_exec_sn
		, .desc					= "sort numeric"
	}
};

int op_validate_ssm(operation * o)
{
	if(o->argsl == 1 || o->argsl == 2 || o->argsl == 3)
	{
		if(o->args[0]->l == 0)
			fail("ssm -- first argument empty");

		if(o->argsl == 1 || o->args[1]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl > 1)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[1]->s);

		o->args[0]->itype = ITYPE_RE;

		if(o->argsl == 3)
		{
			int x;
			for(x = 0; x < o->args[2]->l; x++)
			{
				if(o->args[2]->s[x] == 'i') { }
				else
				{
					fail("ssm -- unknown modifier : %c(0x%02hhx)", o->args[2]->s[x], o->args[2]->s[x]);
				}
			}
		}
	}
	else
	{
		fail("ssm -- invalid arguments : %d", o->argsl);
	}

	finally : coda;
}

int op_validate_ssw(operation * o)
{
	if(o->argsl == 1 || o->argsl == 2 || o->argsl == 3)
	{
		if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
			fail("ssw -- first argument should be i64");
		if(o->argsl >= 2 && o->args[1]->itype != ITYPE_I64)
			fail("ssw -- second argument should be i64");
		if(o->argsl == 3)
		{
			int x;
			for(x = 0; x < o->args[2]->l; x++)
			{
				if(o->args[2]->s[x] == 'i') { }
				else
				{
					fail("ssw -- unknown modifier : %c(0x%02hhx)", o->args[2]->s[x], o->args[2]->s[x]);
				}
			}
		}
	}
	else
	{
		fail("ssw -- invalid arguments : %d", o->argsl);
	}

	finally : coda;
}

int op_validate_snm(operation * o)
{
	if(o->argsl == 1 || o->argsl == 2)
	{
		if(o->args[0]->l == 0)
			fail("snm -- first argument empty");

		if(o->argsl == 1 || o->args[1]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl > 1)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[1]->s);

		o->args[0]->itype = ITYPE_RE;
	}
	else
	{
		fail("snm -- invalid arguments : %d", o->argsl);
	}

	finally : coda;
}

int op_validate_snw(operation * o)
{
	if(o->argsl == 1 || o->argsl == 2)
	{
		if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
			fail("snw -- first argument should be i64");
		if(o->argsl >= 2 && o->args[1]->itype != ITYPE_I64)
			fail("snw -- second argument should be i64");
	}
	else
	{
		fail("snw -- invalid arguments : %d", o->argsl);
	}

	finally : coda;
}

#define STRING_WCASE	1
#define STRING_NCASE	2
#define NUMERIC				3

static int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len, int mode, int win, int win_off, int win_len, struct re * re)
{
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

		int r = 0;

#define FAIL(s)		do { if(s) { dprintf(listwise_err_fd, s); } qfail = 1; return 0; } while(0)

		if(mode == NUMERIC)
		{
			if((As = lstack_getstring(ls, 0, *(int*)A)) == 0)
				FAIL("allocation failure");
			if((Bs = lstack_getstring(ls, 0, *(int*)B)) == 0)
				FAIL("allocation failure");

			if(re || win)
			{
				Asl = strlen(As);
				Bsl = strlen(Bs);
			}
		}
		else
		{
			if(lstack_string(ls, 0, *(int*)A, &As, &Asl))
				FAIL("allocation failure");
			if(lstack_string(ls, 0, *(int*)B, &Bs, &Bsl))
				FAIL("allocation failure");
		}

		if(re)
		{
			if(re_exec(re, As, Asl, 0, ovec, ovec_len))
				FAIL(0);
			if((*ovec)[0] <= 0)
			{
				As = 0;
			}
			else
			{
				As = As + (*ovec)[1];
				Asl = (*ovec)[2] - (*ovec)[1];
			}

			if(re_exec(re, Bs, Bsl, 0, ovec, ovec_len))
				FAIL(0);
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
		else if(win)
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
				As += off;
				Asl = len;
			}

			off = win_off;
			len = win_len;

			if(win_off < 0)
				off = Bsl + win_off;
			if(win_len == 0)
				len = Bsl - win_off;

			if(off >= Bsl || len <= 0)
			{
				Bs = 0;
			}
			else
			{
				Bs += off;
				Bsl = len;
			}
		}

		if(As && Bs)
		{
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
		}
		else if(As)
		{
			r = 1;
		}
		else if(Bs)
		{
			r = -1;
		}

		return r;
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
	int ncase = 0;

	if(o->argsl >= 1)
		ncase = strchr(o->args[0]->s, 'i') != 0;

	return op_exec(o, ls, ovec, ovec_len, ncase ? STRING_NCASE : STRING_WCASE, 0, 0, 0, 0);
}

int op_exec_ssm(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	int ncase = 0;

	if(o->argsl >= 3)
		ncase = strchr(o->args[2]->s, 'i') != 0;

	return op_exec(o, ls, ovec, ovec_len, ncase ? STRING_NCASE : STRING_WCASE, 0, 0, 0, &o->args[0]->re);
}

int op_exec_ssw(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	int win_off = 0;
	int win_len = 0;
	int ncase = 0;

	if(o->argsl >= 1)
		win_off = o->args[0]->i64;
	if(o->argsl >= 2)
		win_len = o->args[1]->i64;
	if(o->argsl >= 3)
		ncase = strchr(o->args[2]->s, 'i') != 0;

	return op_exec(o, ls, ovec, ovec_len, ncase ? STRING_NCASE : STRING_WCASE, 1, win_off, win_len, 0);
}

int op_exec_sn(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, NUMERIC, 0, 0, 0, 0);
}

int op_exec_snm(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, NUMERIC, 0, 0, 0, &o->args[0]->re);
}

int op_exec_snw(operation * o, lstack * ls, int ** ovec, int * ovec_len)
{
	int win_off = 0;
	int win_len = 0;

	if(o->argsl >= 1)
		win_off = o->args[0]->i64;
	if(o->argsl >= 2)
		win_len = o->args[1]->i64;

	return op_exec(o, ls, ovec, ovec_len, NUMERIC, 1, win_off, win_len, 0);
}
