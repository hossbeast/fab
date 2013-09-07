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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

/*

stat operator - replace path entries with related filesystem information

ARGUMENTS
	[0] - replacement expression
	[1] - flags

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
	char space[256];

	// perms user group size modify name
	char * fmt = "%m %u %g %s %t %f";

	int isstat = o->argsl == 2 && o->args[1]->l && strchr(o->args[1]->s, 'H');

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
			for(i = 0; i < strlen(fmt); i++)
			{
				if(((i + 1) < strlen(fmt)) && fmt[i] == '%')
				{
					if(fmt[i + 1] == 'm')
					{
						fatal(lstack_appendf, ls, 0, x, "%c%c%c%c%c%c%c%c%c"
							, st.st_mode & S_IRUSR ? 'r' : '-'
							, st.st_mode & S_IWUSR ? 'w' : '-'
							, st.st_mode & S_IXUSR ? 'x' : '-'
							, st.st_mode & S_IRGRP ? 'r' : '-'
							, st.st_mode & S_IWGRP ? 'w' : '-'
							, st.st_mode & S_IXGRP ? 'x' : '-'
							, st.st_mode & S_IROTH ? 'r' : '-'
							, st.st_mode & S_IWOTH ? 'w' : '-'
							, st.st_mode & S_IXOTH ? 'x' : '-'
						);
					}
					else if(fmt[i + 1] == 'U')
					{
						fatal(lstack_appendf, ls, 0, x, "%u", st.st_uid);
					}
					else if(fmt[i + 1] == 'u')
					{
						char * name = "(none)";
						struct passwd stor;
						struct passwd * pwd;
						if(getpwuid_r(st.st_uid, &stor, space, sizeof(space), &pwd) == 0)
						{
							if(pwd)
							{
								name = pwd->pw_name;
							}
							else
							{
								// name not found
							}
						}
						else if(errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM)
						{
							// name not found
						}
						else
						{
							// other error
							fail("getpwd(%d)=[%d][%s]", st.st_uid, errno, strerror(errno));
						}

						fatal(lstack_appendf, ls, 0, x, "%s", name);
					}
					else if(fmt[i + 1] == 'G')
					{
						fatal(lstack_appendf, ls, 0, x, "%u", st.st_gid);
					}
					else if(fmt[i + 1] == 'g')
					{
						char * name = "(none)";
						struct group stor;
						struct group * grp;
						if(getgrgid_r(st.st_gid, &stor, space, sizeof(space), &grp) == 0)
						{
							if(grp)
							{
								name = grp->gr_name;
							}
							else
							{
								// name not found
							}
						}
						else if(errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM)
						{
							// name not found
						}
						else
						{
							// other error
							fail("getgrp(%d)=[%d][%s]", st.st_uid, errno, strerror(errno));
						}

						fatal(lstack_appendf, ls, 0, x, "%s", name);
					}
					else if(fmt[i + 1] == 't')
					{
						struct tm tm;
						if(localtime_r(&st.st_mtime, &tm) == 0)
							fail("localtime failed");

						size_t z = 0;
						if((z = strftime(space, sizeof(space), "%b %d %T", &tm)) == 0)
							fail("strftime failed");

						fatal(lstack_append, ls, 0, x, space, z);
					}
					else if(fmt[i + 1] == 's')
					{
						fatal(lstack_appendf, ls, 0, x, "%6zu", st.st_size);
					}
					else if(fmt[i + 1] == 'f')
					{
						fatal(lstack_appendf, ls, 0, x, "%s", s);
					}

					i++;
				}
				else
				{
					fatal(lstack_append, ls, 0, x, fmt + i, 1);
				}
			}

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
