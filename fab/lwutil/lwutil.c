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

#include "listwise.h"
#include "listwise/xtra.h"
#include "listwise/object.h"
#include "listwise/generator.h"

#include "lwutil.h"

#include "global.h"
#include "xlinux.h"
#include "log.h"

///
/// static
///

static int ensure(lwx *** stax, int * staxa, int staxp)
{
	// ensure enough lstacks are allocated
	if((*staxa) <= staxp)
	{
		int ns = (*staxa) ?: 10;
		ns = ns * 2 + ns / 2;

		fatal(xrealloc, stax, sizeof(**stax), ns, (*staxa));
		(*staxa) = ns;
	}

	// ensure lstack at this spot is allocated
	if(!(*stax)[staxp])
		fatal(lwx_alloc, &(*stax)[staxp]);

	finally : coda;
}

static int flatten(lwx * lso)
{
	int x;
	int c = 0;

	lwx * lsi = 0;

	// iterate the outer list
	LSTACK_ITERREV(lso, x, goo);
	if(goo)
	{
		char * sp = 0;
		uint8_t st = 0;
		fatal(lstack_getobject, lso, 0, x, &sp, &st);

		if(st == LISTWISE_TYPE_LIST)
		{
			// get reference to the inner list
			lsi = *(void**)sp;

			// flatten the inner list
			fatal(flatten, lsi);

			// delete the inner list from the outer list
			fatal(lstack_delete, lso, 0, x);

			int rl = LSTACK_COUNT(lsi);

			// displace enough entries to insert the inner list
			fatal(lstack_displace, lso, 0, x, rl);

			int i = 0;
			int y;
			LSTACK_ITERATE(lsi, y, goi)
			if(goi)
			{
				char * isp;
				int isl;
				uint8_t ist;
				fatal(lstack_readrow, lsi, 0, y, &isp, &isl, &ist, 0, 0, 0, 0);

				if(ist)
				{
					fatal(lstack_obj_write_alt, lso, 0, x + i, *(void**)isp, ist);
				}
				else
				{
					fatal(lstack_write_alt, lso, 0, x + i, isp, isl);
				}
				i++;
			}
			LSTACK_ITEREND;
		}

		c++;
	}
	LSTACK_ITEREND;

	// as a special case, a list with a single entry which is itself a list inherits
	// the interpolation context of the inner list
	if(c == 1 && lsi)
		lwx_setflags(lso, lwx_getflags(lsi));

	finally : coda;
}

///
/// public
///

int lw_flatten(lwx * ls)
{
	return flatten(ls);
}

int lw_ensure(lwx *** stax, int * staxa, int staxp)
{
	return ensure(stax, staxa, staxp);
}

int lw_reset(lwx *** stax, int * staxa, int staxp)
{
	fatal(ensure, stax, staxa, staxp);
	lwx_reset((*stax)[staxp]);

	finally : coda;
}

int lw_exec(generator * gen, char * tex, lwx ** ls)
{
	// in liblistwise.so
	extern int lstack_exec_internal(generator* g, char** init, int* initls, int initl, lwx ** ls, int dump);

	// flatten first
	fatal(flatten, (*ls));

	// pass through listwise
	log(L_LWVOCAL, "%s", tex);
	fatal(listwise_exec_generator, gen, 0, 0, 0, ls, log_would(L_LWVOCAL));

	finally : coda;
}
