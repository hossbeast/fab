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
#include <listwise/lstack.h>

#include "gn.h"
#include "control.h"

/*

(fab specific) fg operator - select graph nodes

ARGUMENTS
	[0] - options string

OPERATION
  with no arguments, fx is equivalent to fxc - set interpolation mode to INTERPOLATION_DELIM_WS
  with an argument, set interpolation mode to INTERPOLATION_DELIM_CUST

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "fg"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "(fab specific) select graph nodes (p|s|g|t|n)"
	}
	, {}
};

int op_validate(operation* o)
{
	if(o->argsl != 0 && o->argsl != 1)
	{
		dprintf(listwise_err_fd, "fg -- arguments : %d", o->argsl);
		return 1;
	}

	return 0;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	uint32_t set = 0xFFFFFFFF;

	if(o->argsl)
	{
		set = 0;
		if(strstr(o->args[0]->s, "p"))
			set |= (0x01 << GN_DESIGNATION_PRIMARY);
		if(strstr(o->args[0]->s, "s"))
			set |= (0x01 << GN_DESIGNATION_SECONDARY);
		if(strstr(o->args[0]->s, "g"))
			set |= (0x01 << GN_DESIGNATION_GENERATED);
		if(strstr(o->args[0]->s, "t"))
			set |= (0x01 << GN_DESIGNATION_TASK);
		if(strstr(o->args[0]->s, "n"))
			set |= (0x01 << GN_DESIGNATION_GROUP);
	}

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		gn * g = 0;

		if(ls->s[0].s[x].type)
		{
			g = *(void**)ls->s[0].s[x].s;
		}

		if(g)
		{
			if(set & (0x01 << g->designate))
				fatal(lstack_last_set, ls, x);
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}
