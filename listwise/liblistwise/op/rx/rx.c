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
#include <string.h>
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"
#include "listwise/object.h"

#include "xlinux.h"

/*

rx (reflect) operator - replace selected object entries with their reflected properties

ARGUMENTS
	*prop - property name - default is the objects default string property

OPERATION

	1. if nothing selected, select all
	2. foreach selected entry
		3. if that entry is an object, replace the entry with the collection returned by its reflect method

*/

static xapi op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "rx"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_ARGS_CANHAVE | LWOP_WINDOWS_RESET
		, .op_exec			= op_exec
		, .desc					= "replace object entries with their reflected properties"
	}
	, {}
};

xapi op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len, void ** udata)
{
  enter;

	void **		r = 0;
	uint8_t *	rtypes = 0;
	int *			rls = 0;
	int				rl = 0;

	int *			last = 0;
	int				lastl = 0;
	int				lasta = 0;

	char* prop = 0;
	if(o->argsl)
		prop = o->args[0]->s;

	int x;
	int i;
	LSTACK_ITERREV(ls, x, go)
	if(go && ls->s[0].s[x].type)
	{
		listwise_object * def = 0;
		fatal(listwise_lookup_object, ls->s[0].s[x].type, &def);

		if(def)
		{
			ifree(&r);
			ifree(&rtypes);
			ifree(&rls);

			// call the reflection method on the appropriate object
			fatal(def->reflect, *(void**)ls->s[0].s[x].s, prop, &r, &rtypes, &rls, &rl);

			// delete the entry just reflected upon
			fatal(lstack_delete, ls, 0, x);

			for(i = 0; i < lastl; i++)
				last[i]--;

			// displace rl entries at 0:x so we can write to them
			fatal(lstack_displace, ls, 0, x, rl);

			for(i = 0; i < lastl; i++)
				last[i] += rl;

			// write rl new entries starting at x
			for(i = 0; i < rl; i++)
			{
				if(rtypes && rtypes[i])
				{
					fatal(lstack_obj_alt_write, ls, 0, x + i, r[i], rtypes[i]);
				}
				else
				{
					fatal(lstack_alt_writew, ls, 0, x + i, r[i], rls[i]);
				}

				// reallocate last block, if necessary
				if(lastl == lasta)
				{
					int ns = lasta ?: 10;
					ns = ns * 2 + ns / 2;
					fatal(xrealloc, &last, sizeof(*last), ns, lasta);
					lasta = ns;
				}

				last[lastl++] = x + i;
			}
		}
	}
	LSTACK_ITEREND

	for(x = 0; x < lastl; x++)
		fatal(lstack_selection_stage, ls, last[x]);

finally:
	free(r);
	free(rtypes);
	free(rls);
	free(last);
coda;
}
