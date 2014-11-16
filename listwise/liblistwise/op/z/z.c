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

z  : remove selection and windows
sz : remove selection
wz : remove windows

NO ARGUMENTS

OPERATION

 1. select all

*/

operator op_desc[] = {
	  {
		  .s						= "z"
		, .optype				= LWOP_SELECTION_RESET | LWOP_WINDOWS_RESET
		, .mnemonic			= "zero"
		, .desc					= "remove selection and windows"
	}
	, {
		  .s						= "sz"
		, .optype				= LWOP_SELECTION_RESET
		, .mnemonic			= "selection-zero"
		, .desc					= "remove selection"
	}
	, {
		  .s						= "wz"
		, .optype				= LWOP_WINDOWS_RESET
		, .mnemonic			= "windows-zero"
		, .desc					= "remove windows"
	}
	, {}
};
