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
#include <unistd.h>

#include "listwise/operator.h"

#include "liblistwise_control.h"
#include "xmem.h"
#include "canon.h"

/*

can operator - path canonicalization (with canon)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. rewrite the item as a canonicalized path

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "can"
		, .optype				= LWOP_OPERATION_INPLACE | LWOP_OPERATION_FILESYSTEM | LWOP_ARGS_CANHAVE | LWOP_MODIFIERS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "canon"
		, .desc					= "path canonicalization with canon"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 0 && o->argsl != 1 && o->argsl != 2)
		fail("%s -- args : %d", o->op->s, o->argsl);

	finally : coda;
}

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	char space[256];

	char * base = 0;
	int basel = 0;
	uint32_t opts = 0;

	char * cwd = 0;
	int cwdl = 0;

	int x;
	for(x = 0; x < o->argsl; x++)
	{
		if(opts == 0)
		{
			if(o->args[x]->itype == ITYPE_I64)
			{
				opts = o->args[x]->i64;
			}
			else
			{
				int i;
				for(i = 0; i < o->args[x]->l; i++)
				{
					if(o->args[x]->s[i] == 'L') { }
					else if(o->args[x]->s[i] == 'R') { }
					else
						break;
				}

				if(i == o->args[x]->l)
				{
					if(strchr(o->args[x]->s, 'L'))
						opts = CAN_FORCE_DOT | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_NEXT_SYM;
					else
						opts = CAN_FORCE_DOT | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_NEXT_SYM;
				}
			}
		}

		if(opts == 0)
		{
			base = o->args[x]->s;
			basel = o->args[x]->l;
		}
	}

	if(base == 0)
	{
		if((cwd = getcwd(0, 0)) == 0)
			fail("getcwd=[%d][%s]", errno, strerror(errno));
		cwdl = strlen(cwd);

		base = cwd;
		basel = cwdl;
	}

	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_getstring, lx, 0, x, &ss, &ssl);

		// path canonicalization
		size_t zl = 0;
		fatal(canon, ss, ssl, base, basel, space, sizeof(space), &zl, opts);

		// rewrite the row
		fatal(lstack_write, lx, 0, x, space, zl);
	}
	LSTACK_ITEREND

finally:
	free(cwd);
coda;
}