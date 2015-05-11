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

#include <stdarg.h>

#include "listwise.h"
#include "listwise/object.h"
#include "listwise/transform.h"
#include "listwise/logging.h"
#include "listwise/lstack.h"
#include "listwise/exec.h"

#include "lwutil.h"

#include "global.h"
#include "xlinux.h"
#include "logs.h"

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
					fatal(lstack_obj_alt_write, lso, 0, x + i, *(void**)isp, ist);
				}
				else
				{
					fatal(lstack_alt_writew, lso, 0, x + i, isp, isl);
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
/// liblistwise logging bindings
///

int listwise_would(void * token, void * udata)
{
	return log_would(*(uint64_t*)token);
}

void listwise_log(void * token, void * udata, const char * func, const char * file, int line, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
#if DEBUG || DEVEL
	log_vlogf(func, file, line, *(uint64_t*)token, fmt, va);
#else
	log_vlogf(*(uint64_t*)token, fmt, va);
#endif
	va_end(va);
}

///
/// public
///

int lw_flatten(lwx * ls)
{
	xproxy(flatten, ls);
}

int lw_ensure(lwx *** stax, int * staxa, int staxp)
{
	xproxy(ensure, stax, staxa, staxp);
}

int lw_reset(lwx *** stax, size_t * staxa, size_t staxp)
{
	fatal(ensure, stax, staxa, staxp);
	fatal(lwx_reset, (*stax)[staxp]);

	finally : coda;
}

int lw_exec(transform * gen, lwx ** ls)
{
	// flatten first
	fatal(flatten, (*ls));

	// execute
	fatal(listwise_exec_transform, gen, 0, 0, 0, ls);

	finally : coda;
}

static struct listwise_logging * logging = (struct listwise_logging[]) {{
#if DEBUG || DEVEL
      .transform_token  = (uint64_t[]) { L_LWPARSE }
    , .transform_would  = listwise_would
    , .transform_log    = listwise_log
    , .lstack_token     = (uint64_t[]) { L_LWEXEC }
    , .lstack_would     = listwise_would
    , .lstack_log       = listwise_log
    , .exec_token       = (uint64_t[]) { L_LWEXEC }
    , .exec_would       = listwise_would
    , .exec_log         = listwise_log
    , .opinfo_token     = (uint64_t[]) { L_LWOPINFO }
    , .opinfo_would     = listwise_would
    , .opinfo_log       = listwise_log
    , .tokens_token     = (uint64_t[]) { L_LWTOKEN }
    , .tokens_would     = listwise_would
    , .tokens_log       = listwise_log
    , .states_token     = (uint64_t[]) { L_LWSTATE }
    , .states_would     = listwise_would
    , .states_log       = listwise_log
#endif
#if SANITY
    , .sanity_token     = (uint64_t[]) { L_LWSANITY }
    , .sanity_would     = listwise_would
    , .sanity_log       = listwise_log
#endif
}};

void lw_configure_logging()
{
	listwise_logging_configure(logging);
}
