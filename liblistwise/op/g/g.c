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
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"

#include "liblistwise_control.h"

/*

g operator - gobble file contents

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

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "g"
		, .optype				= LWOP_SELECTION_RESET | LWOP_ARGS_CANHAVE | LWOP_OPERATION_PUSHBEFORE | LWOP_OPERATION_FILESYSTEM | LWOP_EMPTYSTACK_YES
		, .op_exec			= op_exec
		, .mnemonic			= "gobble"
		, .desc					= "create new list from file content(s) one row per line"
	}
	, {}
};

static int gobble(lwx* ls, char * path)
{
	int					fd = -1;
	size_t			size = 0;
	void *			addr = MAP_FAILED;

	if((fd = open(path, O_RDONLY)) == -1)
		fail("open(%s)=[%d][%s]", path, errno, strerror(errno));

	if((size = lseek(fd, 0, SEEK_END)) ==  (off_t)-1)
		fail("lseek(%u)=[%d][%s]", 0, errno, strerror(errno));

	if((addr = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		fail("mmap(%zu)=[%d][%s]", size, errno, strerror(errno));

	char * s = addr;
	char * e = 0;
	while((e = strstr(s, "\n")))
	{
		fatal(lstack_add, ls, s, e - s);
		s = e + 1;
	}

finally:
	if(addr != MAP_FAILED)
		munmap(addr, size);

	if(fd != -1)
		close(fd);
coda;
}

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int x;
	fatal(lstack_unshift, ls);

	if(o->argsl)
	{
		for(x = 0; x < o->argsl; x++)
			fatal(gobble, ls, o->args[x]->s);
	}
	else
	{
		LSTACK_ITERATE_FWD(ls, 1, x, 1, go)
		if(go)
		{
			fatal(gobble, ls, lstack_string(ls, 1, x));
		}
		LSTACK_ITEREND
	}

	finally : coda;
}
