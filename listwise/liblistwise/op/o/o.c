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
#include <alloca.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "listwise/operator.h"


/*

o operator - aggregate selections (this OR that)

NO ARGUMENTS

OPERATION

	1. do not reset the "last list" before the next operator
	2. do not excute the implicit "y" after the preceeding operator

	1. do not unstage selections
	2. do not unstage windows

*/

operator op_desc[] = {
	{
		  .s						= "o"
		, .optype				= LWOP_SELECTION_AGGREGATE | LWOP_WINDOWS_AGGREGATE
		, .mnemonic			= "or"
		, .desc					= "aggregate selection and windows across operators"
	}
	, {
		  .s						= "so"
		, .optype				= LWOP_SELECTION_AGGREGATE
		, .mnemonic			= "selection-or"
		, .desc					= "aggregate selection across operators"
	}
	, {
		  .s						= "wo"
		, .optype				= LWOP_WINDOWS_AGGREGATE
		, .mnemonic			= "windows-or"
		, .desc					= "aggregate windows across operators"
	}
	, {}
};