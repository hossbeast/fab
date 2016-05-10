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
#include <string.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "macros.h"
#include "xlinux.h"

/*

wvf operator - window preceeding

NO ARGUMENTS

*/

static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "wvp"
		, .optype				= LWOP_WINDOWS_ACTIVATE | LWOP_SELECTION_STAGE
		, .op_exec			= op_exec
		, .desc					= "window section preceeding first windowed section"
		, .mnemonic			= "window-preceeding"
	}
	, {}
};

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
  enter;

	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		lwx_windows * win;
		int state;
		if((state = lstack_windows_state(lx, x, &win)) == LWX_WINDOWS_SOME)
		{
			// preceeding the first windowed segment
			fatal(lstack_windows_stage, lx, x, 0, win->s[0].o);
			fatal(lstack_selection_stage, lx, x);
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
