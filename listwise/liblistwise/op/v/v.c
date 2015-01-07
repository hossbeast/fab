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

#include <stdlib.h>
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"
#include "listwise/window.h"

#include "macros.h"
#include "xlinux.h"

/*

v  : invert selection
wv : invert windows

NO ARGUMENTS

*/

static int v_exec(operation*, lwx*, int**, int*, void**);
static int wv_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "v"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_ACTIVATE
		, .op_exec			= v_exec
		, .mnemonic			= "invert"
/*
		, .aliases			= (char*[]) { "sv", "selection-invert", 0 }
*/
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

int wv_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	int x;
	LSTACK_ITERATE_FWD(lx, 0, x, 1, 0, go)
	if(go)
	{
		lwx_windows * win;
		int state = lstack_windows_state(lx, x, &win);
		if(state == LWX_WINDOWS_ALL)
		{
			fatal(lstack_windows_stage_nil, lx, x);	// nil
		}
		else
		{
			char * s = 0;
			int sl = 0;
			fatal(lstack_readrow, lx, 0, x, &s, &sl, 0, 1, 0, 0, 0);

			if(state == LWX_WINDOWS_SOME)
			{
				// before the first windowed segment
				if(win->s[0].o)
					fatal(lstack_windows_stage, lx, x, 0, win->s[0].o);

				int i;
				for(i = 1; i < win->l; i++)
				{
					// region following the previous segment and preceeding the current segment
					fatal(lstack_windows_stage, lx, x, win->s[i - 1].o + win->s[i - 1].l, win->s[i].o - (win->s[i - 1].o + win->s[i - 1].l));
				}

				// following the last windowed segment
				if(sl - (win->s[i - 1].o + win->s[i - 1].l))
					fatal(lstack_windows_stage, lx, x, win->s[i - 1].o + win->s[i - 1].l, sl - (win->s[i - 1].o + win->s[i - 1].l));
			}
			else // state == LWX_WINDOWS_NONE
			{
				fatal(lstack_windows_stage, lx, x, 0, sl);
			}
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
