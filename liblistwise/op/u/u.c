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

#include "listwise/operator.h"

#include "liblistwise_control.h"
#include "xstring.h"

#include "parseint.h"

/*

u operator - select stringwise-unique entries from an already-sorted list

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries which are stringwise NEQ to the preceeding entry

*/

static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "u"
		, .optype				= LWOP_SELECTION_ACTIVATE
		, .op_exec			= op_exec
		, .mnemonic			= "unique"
		, .desc					= "select unique (already sorted)"
	}, {}
};

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	int p = -1;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * As = 0;
		int    Asl = 0;
		char * Bs = 0;
		int    Bsl = 0;

		if(p != -1)
		{
			lstack_getbytes(ls, 0, p, &As, &Asl);
			lstack_getbytes(ls, 0, x, &Bs, &Bsl);
		}

		if(p == -1 || xstrcmp(As, Asl, Bs, Bsl, 0))
		{
			fatal(lstack_sel_stage, ls, x);
		}

		p = x;
	}
	LSTACK_ITEREND

	finally : coda;
}
