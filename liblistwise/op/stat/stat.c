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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

/*

stat operator - eXtension Substitution

ARGUMENTS
	[0] - full extension match
	[1] - matching extension string
	 2  - replacement extension string

OPERATION

	1. if nothing selected, select all
	2. foreach selected entry
		2.1. replace each entry with 

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "stat"
		, .optype				= LWOP_SELECTION_READ | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "get filesystem stat information"
	}, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 0 && o->argsl != 1 && o->argsl != 2)
		fail("stat -- arguments : %d", o->argsl);

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int isstat = o->argsl == 2 && o->args[1]->l && strchr(o->args[1]->s, 'H');

	// perms user group size modify name
	char * fmt = "%m %u %g %s %t %f";
	int x;

	if((o->argsl == 1 || o->argsl == 2) && o->args[0]->l)
	{
		if(o->argsl == 2)
		{
			fmt = o->args[0]->s;
		}
		else
		{
			for(x = 0; x < o->args[0]->l; x++)
			{
				if(o->args[0]->s[x] == 'H') { }
				else
				{
					break;
				}
			}

			if(x < o->args[0]->l)
			{
				fmt = o->args[0]->s;
			}
		}
	}

	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		struct stat st;
		int r = 1;
		char * s = lstack_getstring(ls, 0, x);

		if(isstat)
			r = stat(s, &st);
		else
			r = lstat(s, &st);

		if(r == 0)
		{
			// clear this string on the stack
			lstack_clear(ls, 0, x);

			int i;
			for(i = 0; i < fmt; i++)
			{
				if(((i + 1) < fmt) && fmt[i] == '%')
				{
					if(fmt[i + 1] == 'm')
					{

					}
					else if(fmt[i + 1] == 'U')
					{
						fatal(lstack_appendf, ls, 0, x, "%u", st.st_uid);
					}
					else if(fmt[i + 1] == 'u')
					{
						struct passwd pwd;
						getpwuid_r(st.st_uid, &pwd, 
					}
					else if(fmt[i + 1] == 'G')
					{
						fatal(lstack_appendf, ls, 0, x, "%u", st.st_gid);
					}
					else if(fmt[i + 1] == 'g')
					{

					}
					else if(fmt[i + 1] == 't')
					{

					}
					else if(fmt[i + 1] == 'f')
					{

					}
				}
			}
			
			// write the stat string
			lstack_appendf(ls, 0, x, "%zu", st.st_size);

			lstack_appendf(ls, 0, x, " %s", s);

			// record this index was hit
			fatal(lstack_last_set, ls, x);
		}
		else
		{
			dprintf(listwise_err_fd, "%s('%s')=[%d][%s]\n", isstat ? "stat" : "lstat", lstack_getstring(ls, 0, x), errno, strerror(errno));
		}
	}
	LSTACK_ITEREND

	finally : coda;
}
