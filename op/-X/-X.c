/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "control.h"

#include "parseint.h"

/*

-f operator - select entries whose stringvalue is a path referencing a regular file
-d operator - select entries whose stringvalue is a path referencing a directory
-l operator - select entries whose stringvalue is a path referencing a symbolic link

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries referencing a regular file

*/

static int op_exec_f(operation*, lstack*, int**, int*);
static int op_exec_d(operation*, lstack*, int**, int*);
static int op_exec_l(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "-f"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_f
		, .desc					= "select regular files"
	}
	, {
		  .s						= "-d"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_d
		, .desc					= "select directories"
	}
	, {
		  .s						= "-l"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_l
		, .desc					= "select symbolic links"
	}
	, {}
};

static int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len, int (*selector)(struct stat *))
{
	int x;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))	// could not be selected
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));	// whether it is selected
		}

		if(go)
		{
			struct stat st;
			if(stat(lstack_getstring(ls, 0, x), &st) == 0)
			{
				if(selector(&st))
					fatal(lstack_last_set, ls, x);
			}
			else
			{
				dprintf(listwise_err_fd, "stat('%s')=[%d][%s]\n", lstack_getstring(ls, 0, x), errno, strerror(errno));
			}
		}
	}

	finally : coda;
}

int op_exec_f(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return S_ISREG(st->st_mode);
	};

	return op_exec(o, ls, ovec, ovec_len, selector);
}

int op_exec_d(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return S_ISDIR(st->st_mode);
	};

	return op_exec(o, ls, ovec, ovec_len, selector);
}

int op_exec_l(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return S_ISLNK(st->st_mode);
	};

	return op_exec(o, ls, ovec, ovec_len, selector);
}
