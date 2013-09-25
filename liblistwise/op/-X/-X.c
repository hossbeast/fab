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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/operator.h"

#include "liblistwise_control.h"

/*

-f operator - select entries whose stringvalue is a path referencing a regular file
-d operator - select entries whose stringvalue is a path referencing a directory
-l operator - select entries whose stringvalue is a path referencing a symbolic link
-e operator - select entries whose stringvalue is an existing path
-z operator - select entries whose stringvalue is a file with zero-byte size, or a directory with no entries
-r operator - select entries whose stringvalue is a readable filesystem path (by the effective uid/gid of the process)
-w operator - select entries whose stringvalue is a writable filesystem path (by the effective uid/gid of the process)
-x operator - select entries whose stringvalue is a executable filesystem path (by the effective uid/gid of the process)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries referencing a regular file

*/

static int op_exec_f(operation*, lwx*, int**, int*);
static int op_exec_d(operation*, lwx*, int**, int*);
static int op_exec_l(operation*, lwx*, int**, int*);
static int op_exec_e(operation*, lwx*, int**, int*);
static int op_exec_z(operation*, lwx*, int**, int*);
static int op_exec_r(operation*, lwx*, int**, int*);
static int op_exec_w(operation*, lwx*, int**, int*);
static int op_exec_x(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "-f"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_f
		, .desc					= "select regular files"
	}
	, {
		  .s						= "-d"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_d
		, .desc					= "select directories"
	}
	, {
		  .s						= "-l"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_l
		, .desc					= "select symbolic links"
	}
	, {
		  .s						= "-e"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_e
		, .desc					= "select existing paths"
	}
	, {
		  .s						= "-z"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_z
		, .desc					= "select zero-byte files and zero-entry directories"
	}
	, {
		  .s						= "-r"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_r
		, .desc					= "select readable paths"
	}
	, {
		  .s						= "-w"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_w
		, .desc					= "select writable paths"
	}
	, {
		  .s						= "-x"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_OPERATION_FILESYSTEM
		, .op_exec			= op_exec_x
		, .desc					= "select executable paths"
	}
	, {}
};

#define USE_STAT  1
#define USE_LSTAT 2

static int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, int statmode, int (*selector)(struct stat *))
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		struct stat st;
		int r = 1;
		if(statmode == USE_STAT)
		{
			r = stat(lstack_string(ls, 0, x), &st);
		}
		else if(statmode == USE_LSTAT)
		{
			r = lstat(lstack_string(ls, 0, x), &st);
		}

		if(r == 0 || errno == ENOENT)
		{
			if(selector(errno == ENOENT ? 0 : &st))
				fatal(lstack_sel_stage, ls, x);
		}
		else
		{
			dprintf(listwise_err_fd, "%s('%s')=[%d][%s]\n", statmode == USE_STAT ? "stat" : "lstat", lstack_string(ls, 0, x), errno, strerror(errno));
		}
	}
	LSTACK_ITEREND

	finally : coda;
}

int op_exec_f(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return st && S_ISREG(st->st_mode);
	};

	return op_exec(o, ls, ovec, ovec_len, USE_STAT, selector);
}

int op_exec_d(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return st && S_ISDIR(st->st_mode);
	};

	return op_exec(o, ls, ovec, ovec_len, USE_STAT, selector);
}

int op_exec_l(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return st && S_ISLNK(st->st_mode);
	};

	return op_exec(o, ls, ovec, ovec_len, USE_LSTAT, selector);
}

int op_exec_e(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return !!st;
	};

	return op_exec(o, ls, ovec, ovec_len, USE_STAT, selector);
}

int op_exec_z(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int selector(struct stat * st)
	{
		return st && S_ISDIR(st->st_mode) ? st->st_nlink == 0 : st->st_size == 0;
	};

	return op_exec(o, ls, ovec, ovec_len, USE_STAT, selector);
}

int op_exec_r(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(euidaccess(lstack_string(ls, 0, x), R_OK) == 0)
			fatal(lstack_sel_stage, ls, x);
		else if(errno != EACCES && errno != ENOENT)
			dprintf(listwise_err_fd, "euidaccess('%s')=[%d][%s]\n", lstack_string(ls, 0, x), errno, strerror(errno));
	}
	LSTACK_ITEREND

	finally : coda;
}

int op_exec_w(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(euidaccess(lstack_string(ls, 0, x), W_OK) == 0)
			fatal(lstack_sel_stage, ls, x);
		else if(errno != EACCES && errno != ENOENT)
			dprintf(listwise_err_fd, "euidaccess('%s')=[%d][%s]\n", lstack_string(ls, 0, x), errno, strerror(errno));
	}
	LSTACK_ITEREND

	finally : coda;
}

int op_exec_x(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		if(euidaccess(lstack_string(ls, 0, x), X_OK) == 0)
			fatal(lstack_sel_stage, ls, x);
		else if(errno != EACCES && errno != ENOENT)
			dprintf(listwise_err_fd, "euidaccess('%s')=[%d][%s]\n", lstack_string(ls, 0, x), errno, strerror(errno));
	}
	LSTACK_ITEREND

	finally : coda;
}
