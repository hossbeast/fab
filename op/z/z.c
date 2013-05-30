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
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "control.h"

/*

z operator - select none

NO ARGUMENTS

OPERATION

 1. clear selection

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "z"
		, .optype				= LWOP_SELECTION_RESET
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "select none"
	}
	, {}
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	lstack_sel_none(ls);
	return 1;
}
