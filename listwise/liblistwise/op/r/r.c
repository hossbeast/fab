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

#include "listwise/operator.h"
#include "listwise/lwx.h"

/*

r operator - reverse items

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. reverse the items amongst themselves

*/

static xapi op_exec(operation*, lwx*, int**, int*, void**);

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

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
  enter;

	if(lx->sel.active == 0 || lx->sel.active->lease != lx->sel.active_era)
	{
		// all items are selected ; reverse the entire list
		int x;
		for(x = 0; x < (lx->s[0].l / 2); x++)
			fatal(lstack_swaptop, lx, x, lx->s[0].l - 1 - x);
	}
	else if(lx->sel.active->state == LWX_SELECTION_NONE)
	{
		// no items selected ; no-op
	}
	else
	{
		// subset of items is selected ; reverse those items amongst themselves
		int a = 0;	// first selected row
		while((lx->sel.active->s[a/8] & (0x01 << (a%8))) == 0)
			a++;

		int b = (lx->sel.active->sl * 8) + 7;	// last selected row
		while((b/8) >= lx->sel.active->sl || (lx->sel.active->s[b/8] & (0x01 << (b%8))) == 0)
			b--;

		int x;
		for(x = 0; x < (lx->sel.active->l / 2); x++)
		{
			fatal(lstack_swaptop, lx, a++, b--);

			while((lx->sel.active->s[a/8] & (0x01 << (a%8))) == 0)
				a++;

			while((lx->sel.active->s[b/8] & (0x01 << (b%8))) == 0)
				b--;
		}
	}

	finally : coda;
}
