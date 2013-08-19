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
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "liblistwise_control.h"

/*

dj operator - disjoin : split list into a stack where each N consecutive selected
              items comprises a new list

ARGUMENTS

 N - grouping constant, default : 1

 use current selection, ELSE
 use entire top list

OPERATION

 1. push an empty list onto the stack
 2. remove unselected items into the new list
 3. for each N items
   3.1. push an empty list onto the stack
   3.2. remove N items from top list into the new list

AFTERWARDS, the stack of lists looks like this

0)      remainder list - unselected items in list 0 when we started (may be empty)
1 .. n) newly-created lists
n+1 ..) contents of the stack beyond list 0 when we started

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "dj"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_SELECTION_RESET | LWOP_OPERATION_PUSHBEFORE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "split list into a stack of lists"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fail("dj - first argument should be i64");

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int N = 1;
	if(o->argsl)
		N = o->args[0]->i64;

	// where the remainder list (list 0) should be after the loop completes
	// to be contiguous with the newly created lists
	int i = ls->l - 1;

	// number of lists added to the stack by the loop
	int k = 0;

	int j = 0;
	int x;
	for(x = ls->s[0].l - 1; x >= 0; x--)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			go = 0;
			if(ls->sel.sl > (x/8))	// could not be selected
			{
				go = (ls->sel.s[x/8] & (0x01 << (x%8)));	// whether it is selected
			}
		}

		if(go)
		{
			if(j == 0)
			{
				k++;
				fatal(lstack_push, ls);
			}

			fatal(lstack_ensure, ls, ls->l - 1, N - j - 1, 0);
			fatal(lstack_move, ls, ls->l - 1, N - j - 1, 0, x);

			j = (j + 1) % N;
		}
	}

	// possibly shrink the last list created
	if(j)
	{
		memmove(
			  &ls->s[ls->l - 1].s[0]
			, &ls->s[ls->l - 1].s[N - j]
			, j * sizeof(ls->s[0].s[0])
		);
		memmove(
			  &ls->s[ls->l - 1].t[0]
			, &ls->s[ls->l - 1].t[N - j]
			, j * sizeof(ls->s[0].t[0])
		);

		ls->s[ls->l - 1].l = j;
		ls->s[ls->l - 1].a = j;
	}

	// move the remainder list to just before the newly-created lists
	typeof(ls->s[0]) G = ls->s[0];
	memmove(
		  &ls->s[0]
		, &ls->s[1]
		, i * sizeof(ls->s[0])
	);
	ls->s[i] = G;

	// reverse the portion of the stack with the newly-created lists
	for(x = i + 1; x <= ((i + k + 1) / 2); x++)
	{
		int a = x;
		int b = i + 1 + k - x + i;

		typeof(ls->s[0]) T = ls->s[a];
		ls->s[a] = ls->s[b];
		ls->s[b] = T;
	}

	// move the portion of the stack just generated to the top
	//  (the new lists, and the remainder list)
	typeof(ls->s[0])* T = alloca(i * sizeof(ls->s[0]));
	memcpy(T, &ls->s[0], i * sizeof(ls->s[0]));

	memmove(
		  &ls->s[0]
		, &ls->s[i]
		, (k + 1) * sizeof(ls->s[0])
	);

	// move the displaced portion to the end
	memcpy(
		  &ls->s[k + i - ((i % 2) ? 0 : 1)]
		, T
		, i * sizeof(ls->s[0])
	);

	// selection reset
	fatal(lstack_sel_all, ls);

	finally : coda;
}





