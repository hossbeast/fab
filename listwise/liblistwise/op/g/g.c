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

#include "xapi.h"
#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

#include "listwise/operator.h"
#include "listwise/fs.h"

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

static xapi op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "g"
		, .optype				= LWOP_OPERATION_PUSHBEFORE | LWOP_ARGS_CANHAVE | LWOP_MODIFIERS_CANHAVE | LWOP_OPERATION_FILESYSTEM | LWOP_EMPTYSTACK_YES
		, .op_exec			= op_exec
		, .mnemonic			= "gobble"
		, .desc					= "create new list from file content(s) at one row per line"
	}
	, {}
};

static xapi gobble(lwx* lx, char * path, char * fmt, char * flags, void ** udata)
{
  enter;

	char space[256];
	size_t sz = 0;

	char space2[32];

	int					fd = -1;
	off_t				size = 0;
	void *			addr = MAP_FAILED;
	char *			xref = 0;

	if(fmt)
	{
		fatal(fs_statfmt, path, 0, fmt, flags, space, sizeof(space), &sz, udata);
		xref = strstr(space, "%x");
	}

	fatal(xopen, path, O_RDONLY, &fd);
	fatal(xlseek, fd, 0, SEEK_END, &size);

	if((addr = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		if(errno == EACCES || errno == ENODEV)
		{
			/* this file cannot be mmapped - should implement a read/loop for this case */
		}
		else
		{
			tfail(perrtab_SYS, errno);
		}
	}

	if(addr != MAP_FAILED)
	{
		char * s = addr;
		char * e = 0;
		int x = 0;
		while((e = strstr(s, "\n")))
		{
			if(sz)
			{
				if(xref)
				{
					snprintf(space2, sizeof(space2), "%5d", ++x);
					memcpy(xref, space2, 5);
				}

				fatal(lstack_addf, lx, "%.*s %.*s", (int)sz, space, (int)(e - s), s);
			}
			else
			{
				fatal(lstack_addw, lx, s, e - s);
			}

			s = e + 1;
		}
	}

finally:
	if(addr != MAP_FAILED)
		munmap(addr, size);

	if(fd != -1)
		close(fd);

  xapi_infos("path", path);
  xapi_infof("size", "%d", size);
coda;
}

xapi op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
  enter;

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
				// listwise/fs flags
				if(o->args[x]->s[i] == 'L') { }
				else if(o->args[x]->s[i] == 'C') { }
				else if(o->args[x]->s[i] == 'F') { }

				// gobble-specific flags
				else if(o->args[x]->s[i] == 'S') { }
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

	if(flags && strchr(flags, 'S'))
		fmt = "%F:%x:";

	fatal(lstack_unshift, lx);

	LSTACK_ITERATE_FWD(lx, 1, x, 1, 1, go)
	if(go)
	{
		char * zs = 0;
		fatal(lstack_string, lx, 1, x, &zs);
		fatal(gobble, lx, zs, fmt, flags, udata);
	}
	LSTACK_ITEREND

	finally : coda;
}
