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

#include <listwise/operator.h>

/*

r operator - reverse items

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. reverse the items amongst themselves

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "r"
		, .optype				= LWOP_SELECTION_READ
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "reverse selected items"
	}
	, {}
};

int op_validate(operation* o)
{
	return 1;
}

void swap(lstack* ls, int a, int b)
{
	typeof(ls->s[0].s[0]) t = ls->s[0].s[a];
	ls->s[0].s[a]						= ls->s[0].s[b];
	ls->s[0].s[b]						= t;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(ls->sel.all || ls->sel.l == ls->s[0].l)
	{
		int x;
		for(x = 0; x < (ls->s[0].l / 2); x++)
			swap(ls, x, ls->s[0].l - 1 - x);
	}
	else
	{
		int a = 0;
		while((ls->sel.s[a/8] & (0x01 << (a%8))) == 0)
			a++;

		int b = (ls->sel.sl * 8) + 7;
		while((b/8) >= ls->sel.sl || (ls->sel.s[b/8] & (0x01 << (b%8))) == 0)
			b--;

		int x;
		for(x = 0; x < (ls->sel.l / 2); x++)
		{
			swap(ls, a++, b--);

			while((ls->sel.s[a/8] & (0x01 << (a%8))) == 0)
				a++;

			while((ls->sel.s[b/8] & (0x01 << (b%8))) == 0)
				b--;
		}
	}

	return 1;
}
