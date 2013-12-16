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
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/internal.h"

#include "xmem.h"

/*

rp operator - path canonicalization (with realpath)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. rewrite the item as a canonicalized path

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "rp"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_OPERATION_INPLACE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec
		, .mnemonic			= "realpath"
		, .desc					= "path canonicalization with realpath"
	}
	, {}
};

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	char * ss = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * s;
		int l;
		fatal(lstack_getstring, ls, 0, x, &s, &l);

		xfree(&ss);
		if((ss = realpath(s, 0)))
		{
			fatal(lstack_write
				, ls
				, 0
				, x
				, ss
				, strlen(ss)
			);
			fatal(lstack_sel_stage, ls, x);
		}
		else if(errno == ENOENT || errno == ENOTDIR)
		{
#if DEBUG
			dprintf(listwise_debug_fd, "realpath(%.*s)=[%d][%s]\n", l, s, errno, strerror(errno));
#endif
		}
		else
		{
			sysfatality("realpath");
		}
	}
	LSTACK_ITEREND

finally:
	free(ss);
coda;
}
