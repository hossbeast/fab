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

#include <listwise.h>
#include <listwise/operator.h>

#include "interpolate.h"

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
static int op_exec_fi(operation*, lstack*, int**, int*);
static int op_exec_fic(operation*, lstack*, int**, int*);
static int op_exec_fiw(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "fi"
		, .optype				= LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_fi
		, .op_exec			= op_exec_fi
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
		dprintf(listwise_err_fd, "fi -- arguments : %d", o->argsl);
		return 0;
	}

	return 1;
}

int op_validate_fic(operation* o)
{
	if(o->argsl != 0)
	{
		dprintf(listwise_err_fd, "fic -- arguments : %d", o->argsl);
		return 0;
	}

	return 1;
}

int op_validate_fiw(operation* o)
{
	if(o->argsl != 0)
	{
		dprintf(listwise_err_fd, "fiw -- arguments : %d", o->argsl);
		return 0;
	}

	return 1;
}

int op_exec_fi(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	if(o->argsl == 0)
	{
		ls->flags = INTERPOLATE_DELIM_WS;
	}
	else if(o->argsl == 1)
	{
		ls->flags = INTERPOLATE_DELIM_CUST;
		free(ls->ptr);
		ls->ptr = strdup(o->args[0]->s);
	}

	return 1;
}

int op_exec_fic(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	ls->flags = INTERPOLATE_DELIM_WS;

	return 1;
}

int op_exec_fiw(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	ls->flags = INTERPOLATE_ADJOIN;

	return 1;
}
