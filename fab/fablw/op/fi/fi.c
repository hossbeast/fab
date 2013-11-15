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

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/xtra.h"

#include "interpolate.h"

#include "fab_control.h"

/*

(fab specific) fi operator - set list interpolation mode

ARGUMENTS
	[0] - delimiter string

OPERATION
  with no arguments, fi is equivalent to fic - set interpolation mode to INTERPOLATION_DELIM_WS
  with an argument, set interpolation mode to INTERPOLATION_DELIM_CUST

*/

static int op_validate_fi(operation* o);
static int op_validate_fic(operation* o);
static int op_validate_fiw(operation* o);
static int op_exec_fi(operation*,  lwx*, int**, int*);
static int op_exec_fic(operation*, lwx*, int**, int*);
static int op_exec_fiw(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "fi"
		, .optype				= LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_fi
		, .op_exec			= op_exec_fi
		, .mnemonic			= "fab-interpolation"
		, .desc					= "(fab specific) list interpolation mode"
	}
	, {
		  .s						= "fic"
		, .optype				= LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_fic
		, .op_exec			= op_exec_fic
		, .desc					= "(fab specific) list interpolation mode - comma"
	}
	, {
		  .s						= "fiw"
		, .optype				= LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_fiw
		, .op_exec			= op_exec_fiw
		, .desc					= "(fab specific) list interpolation mode - whitespace"
	}
	, {}
};

int op_validate_fi(operation* o)
{
	if(o->argsl != 0 && o->argsl != 1)
	{
		fail("fi -- arguments : %d", o->argsl);
	}

	finally : coda;
}

int op_validate_fic(operation* o)
{
	if(o->argsl != 0)
	{
		fail("fic -- arguments : %d", o->argsl);
	}

	finally : coda;
}

int op_validate_fiw(operation* o)
{
	if(o->argsl != 0)
	{
		fail("fiw -- arguments : %d", o->argsl);
	}

	finally : coda;
}

int op_exec_fi(operation* o, lwx * lx, int** ovec, int* ovec_len)
{
	if(o->argsl == 0)
	{
		lwx_setflags(lx, INTERPOLATE_DELIM_WS);
	}
	else if(o->argsl == 1)
	{
		lwx_setflags(lx, INTERPOLATE_DELIM_CUST);
		free(lwx_getptr(lx));
		lwx_setptr(lx, strdup(o->args[0]->s));
	}

	return 0;
}

int op_exec_fic(operation* o, lwx * lx, int** ovec, int* ovec_len)
{
	lwx_setflags(lx, INTERPOLATE_DELIM_WS);

	return 0;
}

int op_exec_fiw(operation* o, lwx * lx, int** ovec, int* ovec_len)
{
	lwx_setflags(lx, INTERPOLATE_ADJOIN);

	return 0;
}
