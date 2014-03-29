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

#include "internal.h"

/*

r operator - reverse items

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. reverse the items amongst themselves

*/

static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "r"
		, .optype				= 0
		, .op_exec			= op_exec
		, .mnemonic			= "reverse"
		, .desc					= "reverse rows"
	}
	, {}
};

void swap(lwx* ls, int a, int b)
{
	typeof(ls->s[0].s[0]) t = ls->s[0].s[a];
	ls->s[0].s[a]						= ls->s[0].s[b];
	ls->s[0].s[b]						= t;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	if(ls->sel.active == 0)
	{
		// all items are selected ; reverse the entire list
		int x;
		for(x = 0; x < (ls->s[0].l / 2); x++)
			swap(ls, x, ls->s[0].l - 1 - x);
	}
	else if(ls->sel.active->nil)
	{
		// no items selected ; no-op
	}
	else
	{
		// subset of items is selected ; reverse those items among themselves
		int a = 0;
		while((ls->sel.active->s[a/8] & (0x01 << (a%8))) == 0)
			a++;

		int b = (ls->sel.active->sl * 8) + 7;
		while((b/8) >= ls->sel.active->sl || (ls->sel.active->s[b/8] & (0x01 << (b%8))) == 0)
			b--;

		int x;
		for(x = 0; x < (ls->sel.active->l / 2); x++)
		{
			swap(ls, a++, b--);

			while((ls->sel.active->s[a/8] & (0x01 << (a%8))) == 0)
				a++;

			while((ls->sel.active->s[b/8] & (0x01 << (b%8))) == 0)
				b--;
		}
	}

	finally : coda;
}
