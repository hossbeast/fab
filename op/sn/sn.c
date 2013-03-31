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
#include <string.h>

#include <listwise/operator.h>

#include "parseint.h"

/*

sn operator - sort numbers (in ascending order)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. sort the items amongst themselves

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype					= LWOP_SELECTION_READ
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "sort numeric"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int * mema = alloca((ls->sel.l ?: ls->s[0].l) * sizeof(mema[0]));
	int * memb = 0;

	int x;
	int i = 0;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.l <= (x/8))
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));
		}

		if(go)
			mema[i++] = x;
	}	

	memb = alloca(i * sizeof(memb[0]));
	memcpy(memb, mema, i * sizeof(memb[0]));

	int compar(const void * A, const void * B)
	{
		intmax_t Aval;
		intmax_t Bval;

		if(parseint(lstack_getstring(ls, 0, *(int*)A), SCNdMAX, INTMAX_MIN, INTMAX_MAX, 0, 0xFF, &Aval, 0))
		{
			if(parseint(lstack_getstring(ls, 0, *(int*)B), SCNdMAX, INTMAX_MIN, INTMAX_MAX, 0, 0xFF, &Bval, 0))
			{
				return Aval - Bval;
			}
		}

		return 0;
	}

	qsort(mema, i, sizeof(mema[0]), compar);

	typeof(ls->s[0].s[0]) * T = alloca(ls->s[0].l * sizeof(T[0]));
	memcpy(T, ls->s[0].s, ls->s[0].l * sizeof(T[0]));

	for(x = 0; x < i; x++)
		ls->s[0].s[memb[x]] = T[mema[x]];

	return 1;
}





