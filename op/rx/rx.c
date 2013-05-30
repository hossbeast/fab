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
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>
#include <listwise/object.h>

#include "control.h"
#include "xmem.h"

/*

rx (reflect) operator - replace selected object entries with their reflected properties

ARGUMENTS
	*prop - property name - default is the objects default string property

OPERATION

	1. if nothing selected, select all
	2. foreach selected entry
		3. if that entry is an object, replace the entry with the collection returned by its reflect method

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "rx"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_ARGS_CANHAVE
		, .op_exec			= op_exec
		, .desc					= "replace object entries with their reflected properties"
	}
	, {}
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
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
	for(x = ls->s[0].l - 1; x >= 0; x--)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			go = 0;
			if(ls->sel.sl > (x/8))
			{
				go = (ls->sel.s[x/8] & (0x01 << (x%8)));
			}
		}

		if(go && ls->s[0].s[x].type)
		{
			listwise_object * def = 0;
			fatal(listwise_lookup_object, ls->s[0].s[x].type, &def);

			if(def)
			{
				xfree(&r);
				xfree(&rtypes);
				xfree(&rls);

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
						fatal(lstack_obj_write_alt, ls, 0, x + i, r[i], rtypes[i]);
					}
					else
					{
						fatal(lstack_write_alt, ls, 0, x + i, r[i], rls[i]);
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
	}

	for(x = 0; x < lastl; x++)
		fatal(lstack_last_set, ls, last[x]);

finally:
	free(r);
	free(rtypes);
	free(rls);
	free(last);
coda;
}
