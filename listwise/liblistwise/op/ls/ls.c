/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include "listwise/operator.h"
#include "listwise/lwx.h"
#include "listwise/logging.h"

#include "parseint.h"
#include "xlinux.h"

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

static int op_exec_ls(operation*, lwx*, int**, int*, void**);
static int op_exec_lsr(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "ls"
		, .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_PUSHBEFORE | LWOP_OPERATION_FILESYSTEM | LWOP_EMPTYSTACK_YES
		, .op_exec			= op_exec_ls
		, .mnemonic			= "directory-listing"
		, .desc					= "create new list from directory listing(s)"
	}
	, {
		  .s						= "lsr"
		, .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_PUSHBEFORE | LWOP_OPERATION_FILESYSTEM | LWOP_EMPTYSTACK_YES
		, .op_exec			= op_exec_lsr
		, .mnemonic			= "directory-listing-recursive"
		, .desc					= "create new list from recursive directory listing(s)"
	}
	, {}
};

static int listing(lwx* ls, char * s, int recurse, void ** udata)
{
	DIR * dd = 0;
	if((dd = opendir(s)))
	{
		struct dirent ent;
		struct dirent * entp = 0;
		while(1)
		{
			fatal(xreaddir_r, dd, &ent, &entp);
			if(entp)
			{
				if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
				{
					fatal(lstack_addf, ls, "%s/%s", s, entp->d_name);

					if(recurse)
					{
						char * zs = 0;
						fatal(lstack_string, ls, 0, ls->s[0].l - 1, &zs);
						fatal(listing, ls, zs, recurse, udata);
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	else if(errno == ENOTDIR || errno == ENOENT)
	{
		lw_log_opinfo("opendir('%s')=[%d][%s]", s, errno, strerror(errno));
	}
	else
	{
		fatal(lstack_adds, ls, s);
	}

finally:
	if(dd)
		closedir(dd);
coda;
}

static int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, int recurse, void ** udata)
{
	int x;
	fatal(lstack_unshift, ls);

	if(o->argsl)
	{
		for(x = 0; x < o->argsl; x++)
			fatal(listing, ls, o->args[x]->s, recurse, udata);
	}
	else
	{
		LSTACK_ITERATE_FWD(ls, 1, x, 1, 1, go)
		if(go)
		{
			char * zs = 0;
			fatal(lstack_string, ls, 1, x, &zs);
			fatal(listing, ls, zs, recurse, udata);
		}
		LSTACK_ITEREND
	}

	finally : coda;
}

int op_exec_ls(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, ls, ovec, ovec_len, 0, udata);
}

int op_exec_lsr(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
	xproxy(op_exec, o, ls, ovec, ovec_len, 1, udata);
}
