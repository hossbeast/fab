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
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

#include "listwise/operator.h"
#include "listwise/fs.h"

#include "liblistwise_control.h"

#include "canon.h"
#include "parseint.h"
#include "xmem.h"
#include "color.h"

/*

stat operator - replace path entries with related filesystem information

ARGUMENTS
	[0] - printf-style expression (taken after find)
	[1] - flags

OPERATION

	1. foreach selected entry
    1.1. render expression using directives and escapes against the entry as a path
		1.1. replace the entry with the resulting string

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "stat"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE | LWOP_OPERATION_FILESYSTEM
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "get filesystem properties"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 0 && o->argsl != 1 && o->argsl != 2)
		fail("%s -- arguments : %d", o->op->s, o->argsl);

	finally : coda;
}

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
{
	// tmp space
	char space[256];

	// format and flags
	char * fmt = 0;
	char * flags = 0;

	// find format and flags
	int x;
	for(x = 0; x < o->argsl; x++)
	{
		int i = o->args[x]->l;
		if(fmt == 0)
		{
			for(i = 0; i < o->args[x]->l; i++)
			{
				if(o->args[x]->s[i] == 'L') { }
				else if(o->args[x]->s[i] == 'C') { }
				else if(o->args[x]->s[i] == 'F') { }
				else
				{
					break;
				}
			}
		}

		if(i < o->args[x]->l)
			fmt = o->args[x]->s;
		else
			flags = o->args[x]->s;
	}

	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_getstring, lx, 0, x, &ss, &ssl);

		// resolve the stat format
		size_t sz;
		fatal(fs_statfmt, ss, ssl, fmt, flags, space, sizeof(space), &sz);

		if(sz)
		{
			// rewrite the row
			fatal(lstack_write, lx, 0, x, space, sz);

			// record this index
			fatal(lstack_sel_stage, lx, x);
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}
