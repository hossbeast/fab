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
#include <string.h>
#include <alloca.h>

#include "listwise/operator.h"

#include "xlinux.h"

/*

y  : activate staged selections and windows
sy : activate staged selections
wy : activate staged windows

NO ARGUMENTS

OPERATION

	1. activate those windows staged by the preceeding operator
	2. activate those selections staged by the preceeding operator

*/

operator op_desc[] = {
	{
		  .s						= "y"
		, .optype				= LWOP_SELECTION_ACTIVATE | LWOP_WINDOWS_ACTIVATE
		, .desc					= "activate staged selection and windows"
	}
	, {
		  .s						= "sy"
		, .optype				= LWOP_SELECTION_ACTIVATE
		, .desc					= "activate staged selection"
	}
	, {
		  .s						= "wy"
		, .optype				= LWOP_WINDOWS_ACTIVATE
		, .desc					= "activate staged windows"
	}, {}
};
