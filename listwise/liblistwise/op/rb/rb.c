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
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "listwise/operator.h"

#include "canon.h"
#include "xlinux.h"

/*

rb operator - path rebasing

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. rebase the row as a string with the specified base path

*/

static xapi op_validate(operation* o);
static xapi op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "rb"
		, .optype				= LWOP_OPERATION_INPLACE | LWOP_ARGS_CANHAVE | LWOP_MODIFIERS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "rebase"
		, .desc					= "path rebasing"
	}
	, {}
};

xapi op_validate(operation* o)
{
  enter;

	if(o->argsl != 0 && o->argsl != 1 && o->argsl != 2)
		failf(LISTWISE_ARGSNUM, "expected : 0 1 or 2, actual : %d", o->op->s, o->argsl);

	finally : coda;
}

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
  enter;

	char space[256];
	char space2[256];

	// absolute base path to rebase TO
	char * base = 0;
	int basel = 0;

	// modifiers string
	char * mods = 0;
	int modsl = 0;

	// current working directory
	char * cwd = 0;
	int cwdl = 0;

	// whether base is a relative path (and therefore requires canonicalization)
	int isrelative = 0;

	int x;
	for(x = 0; x < o->argsl; x++)
	{
		if(mods == 0)
		{
			int i;
			for(i = 0; i < o->args[x]->l; i++)
			{
				if(o->args[x]->s[i] == 'R') { }
				else
					break;
			}

			if(i == o->args[x]->l)
			{
				mods = o->args[x]->s;
				modsl = o->args[x]->l;
			}
		}

		if(mods == 0)
		{
			base = o->args[x]->s;
			basel = o->args[x]->l;
		}
	}

	for(x = 0; x < modsl; x++)
	{
		if(mods[x] == 'R')
			isrelative = 1;
	}

	if(base == 0 || isrelative)
	{
		fatal(xgetcwd, 0, 0, &cwd);
		cwdl = strlen(cwd);

		if(base == 0)
		{
			base = cwd;
			basel = cwdl;
		}
		if(isrelative)
		{
			// base is relative - canonicalize it so it is absolute
			size_t zl;
			fatal(canon, base, basel, cwd, cwdl, space2, sizeof(space2), &zl, CAN_FORCE_DOT | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_NEXT_SYM);
			
			base = space2;
			basel = zl;
		}
	}

	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_getstring, lx, 0, x, &ss, &ssl);

		// rebase path
		size_t zl;
		fatal(rebase, ss, ssl, base, basel, space, sizeof(space), &zl);

		// rewrite the row
		fatal(lstack_writew, lx, 0, x, space, zl);
	}
	LSTACK_ITEREND

finally:
	free(cwd);
coda;
}
