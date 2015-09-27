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

/*

vf operator - select following

NO ARGUMENTS

OPERATION

 1. select items following last selected item

*/

static xapi op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "vf"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_RESET
		, .op_exec			= op_exec
		, .desc					= "select following"
	}
	, {}
};

xapi op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
  enter;

	if(ls->l)
	{
		int i = -1;

		int x;
		LSTACK_ITERATE(ls, x, go);
		if(go)
		{
			i = x;
		}
		LSTACK_ITEREND;

		if(i > -1)
		{
			if(ls->l > 0)
			{
				for(x = 0; x < ls->s[0].l; x++)
				{
					if(x > i)
					{
						fatal(lstack_selection_stage, ls, x);
					}
				}
			}
		}
	}

	finally : coda;
}
