/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "macros.h"
#include "xlinux.h"

/*

v  : invert selection and windows
sv : invert selection
wv : invert windows

NO ARGUMENTS

OPERATION

 1. invert selectedness of each item

*/

static int v_exec(operation*, lwx*, int**, int*, void**);
static int sv_exec(operation*, lwx*, int**, int*, void**);
static int wv_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "v"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_ACTIVATE
		, .op_exec			= v_exec
		, .mnemonic			= "invert"
		, .desc					= "invert selection and windows"
	}
	, {
		  .s						= "sv"
		, .optype				= LWOP_SELECTION_ACTIVATE
		, .op_exec			= sv_exec
		, .mnemonic			= "selection-invert"
		, .desc					= "invert selection"
	}
	, {
		  .s						= "wv"
		, .optype				= LWOP_WINDOWS_ACTIVATE
		, .op_exec			= wv_exec
		, .mnemonic			= "windows-invert"
		, .desc					= "invert windows"
	}
	, {}
};

int v_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE(ls, x, go);
	if(!go)
	{
		fatal(lstack_selection_stage, ls, x);
	}
	LSTACK_ITEREND;

	finally : coda;
}

int sv_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE(ls, x, go);
	if(!go)
	{
		fatal(lstack_selection_stage, ls, x);
	}
	LSTACK_ITEREND;

	finally : coda;
}

int wv_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		lwx_windows * win;
		if(lstack_windows_state(lx, x, &win) == SOME)
		{
			char * s = 0;
			int sl = 0;
			fatal(lstack_readrow, lx, 0, x, &s, &sl, 0, 1, 0, 0, 0);

			// before the first windowed segment
			fatal(lstack_window_stage, lx, x, 0, win->s[0].o);

			int i;
			for(i = 1; i < win->l; i++)
			{
				// region following the previous segment and preceeding the current segment
				fatal(lstack_window_stage, lx, x, win->s[i - 1].o + win->s[i - 1].l, win->s[i].o - (win->s[i - 1].o + win->s[i - 1].l));
			}

			// following the last windowed segment
			fatal(lstack_window_stage, lx, x, win->s[i - 1].o + win->s[i - 1].l, sl - (win->s[i - 1].o + win->s[i - 1].l));
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
