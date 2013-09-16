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
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "liblistwise_control.h"

#include "parseint.h"

/*

ls operator - directory listing

ARGUMENTS

 has arguments - use each
 no arguments  - use current selection
 no selection  - use entire top list

OPERATION

 1. push an empty list onto the stack

 2. for each argument
      argument is the path to a directory
      for each item in the the directory listing
        append path to the item (relative to the argument given)

lsr operator - recursive directory listing

ARGUMENTS

 has arguments - use each
 no arguments  - use current selection
 no selection  - use entire top list

OPERATION

 1. push an empty list onto the stack

 2. for each argument
      2.1 if argument is the path to a directory
          2.2 for each item in the the directory listing
              2.3 append path to the item (relative to the argument given)
							2.4 repeat 2.2 for this item

*/

static int op_validate(operation* o);
static int op_exec_ls(operation*, lstack*, int**, int*);
static int op_exec_lsr(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "ls"
		, .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_PUSHBEFORE | LWOP_OPERATION_FILESYSTEM | LWOP_EMPTYSTACK_YES
		, .op_validate	= op_validate
		, .op_exec			= op_exec_ls
		, .desc					= "create new list from directory listing(s)"
	}
	, {
		  .s						= "lsr"
		, .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_PUSHBEFORE | LWOP_OPERATION_FILESYSTEM | LWOP_EMPTYSTACK_YES
		, .op_validate	= op_validate
		, .op_exec			= op_exec_lsr
		, .desc					= "create new list from recursive directory listing(s)"
	}
	, {}
};

int op_validate(operation* o)
{
	return 0;
}

static int listing(lstack* ls, char * s, int recurse)
{
	DIR * dd = 0;
	if((dd = opendir(s)))
	{
		struct dirent ent;
		struct dirent * entp = 0;
		int r = 0;
		while(1)
		{
			if((r = readdir_r(dd, &ent, &entp)) == 0)
			{
				if(entp)
				{
					if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
					{
						fatal(lstack_addf, ls, "%s/%s", s, entp->d_name);

						if(recurse)
							fatal(listing, ls, lstack_string(ls, 0, ls->s[0].l - 1), recurse);
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				fail("readdir('%s')=[%d][%s]", s, r, strerror(r));
			}
		}
	}
	else if(errno != ENOTDIR)
	{
		dprintf(listwise_err_fd, "opendir('%s')=[%d][%s]\n", s, errno, strerror(errno));
	}

finally:
	closedir(dd);
coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len, int recurse)
{
	int x;
	fatal(lstack_unshift, ls);

	if(o->argsl)
	{
		for(x = 0; x < o->argsl; x++)
			fatal(listing, ls, o->args[x]->s, recurse);
	}
	else
	{
		for(x = 0; x < ls->s[1].l; x++)
		{
			int go = 1;
			if(!ls->sel.all)
			{
				if(ls->sel.sl <= (x/8))
					break;

				go = (ls->sel.s[x/8] & (0x01 << (x%8)));
			}

			if(go)
				fatal(listing, ls, lstack_string(ls, 1, x), recurse);
		}
	}

	finally : coda;
}

int op_exec_ls(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 0);
}

int op_exec_lsr(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len, 1);
}
