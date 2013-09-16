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
#include <alloca.h>

#include <listwise/operator.h>

#include "liblistwise_control.h"
#include "xmem.h"

/*

ys operator - activate those selections staged by the previous operation
yw operator - activate those windows staged by the previous operator

NO ARGUMENTS

OPERATION

	1. activate those windows staged by the preceeding operator
	2. activate those selections staged by the preceeding operator

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "y"
		, .optype				= LWOP_SELECTION_RESET
		, .op_exec			= op_exec
		, .desc					= "activate staged selections and windows"
	}, {}
};

int op_exec_yw(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return lstack_windows_ratify(ls);
}

int op_exec_ys(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return lstack_sel_ratify(ls);
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(op_exec_yw(o, ls, ovec, ovec_len) != 0)
		return 1;

	if(op_exec_ys(o, ls, ovec, ovec_len) != 0)
		return 1;

	return 0;
}
