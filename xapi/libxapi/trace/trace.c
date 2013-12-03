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

#include <stdio.h>
#include <string.h>

#include "xapi/internal.h"

///
/// backtrace generation
///

#define SAY(...) z += snprintf(dst + z, sz - z, __VA_ARGS__)

static size_t frame_trace(char * const dst, const size_t sz, int x, int loc)
{
	size_t z = 0;

	z += snprintf(dst + z, sz - z, "at ");
	z += xapi_frame_function(dst + z, sz - z, x);
	if(callstack.v[x]->info.l)
	{
		z += snprintf(dst + z, sz - z, "(");
		z += xapi_frame_info(dst + z, sz - z, x);
		z += snprintf(dst + z, sz - z, ")");
	}

	if(loc)
	{
		z += snprintf(dst + z, sz - z, " in ");
		z += xapi_frame_location(dst + z, sz - z, x);
	}

	return z;
}

///
/// API
///

size_t API xapi_frame_error(char * const dst, const size_t sz, int x)
{
	size_t z = 0;

	if(callstack.v[x]->etab && callstack.v[x]->code > 0 && callstack.v[x]->msg)
	{
		SAY("[%s:%s] %.*s"
			, callstack.v[x]->etab->name
			, callstack.v[x]->etab->v[callstack.v[x]->code].name
			, callstack.v[x]->msgl, callstack.v[x]->msg 
		);
	}
	else if(callstack.v[x]->etab && callstack.v[x]->code > 0)
	{
		SAY("[%s:%s] %s"
			, callstack.v[x]->etab->name
			, callstack.v[x]->etab->v[callstack.v[x]->code].name
			, callstack.v[x]->etab->v[callstack.v[x]->code].desc
		);
	}
	else if(callstack.v[x]->etab && callstack.v[x]->msg)
	{
		SAY("[%s] %.*s"
			, callstack.v[x]->etab->name
			, callstack.v[x]->msgl, callstack.v[x]->msg 
		);
	}
	else if(callstack.v[x]->code > 0 && callstack.v[x]->msg)
	{
		SAY("[%d] %.*s"
			, callstack.v[x]->code
			, callstack.v[x]->msgl, callstack.v[x]->msg 
		);
	}
	else if(callstack.v[x]->code > 0)
	{
		SAY("[%d]", callstack.v[x]->code);
	}

	return z;
}

size_t API xapi_frame_function(char * const dst, const size_t sz, int x)
{
	size_t z = 0;
	SAY("%s", callstack.v[x]->func);

	return z;
}

size_t API xapi_frame_location(char * const dst, const size_t sz, int x)
{
	size_t z = 0;
	SAY("%s:%d", callstack.v[x]->file, callstack.v[x]->line);

	return z;
}

size_t API xapi_frame_info(char * const dst, const size_t sz, int x)
{
	size_t z = 0;

	int y;
	for(y = 0; y < callstack.v[x]->info.l; y++)
	{
		if(y)
			SAY(", ");

		SAY("%.*s=%.*s"
			, callstack.v[x]->info.v[y].kl
			, callstack.v[x]->info.v[y].ks
			, callstack.v[x]->info.v[y].vl
			, callstack.v[x]->info.v[y].vs
		);
	}

	return z;
}

size_t API xapi_frame_trace(char * const dst, const size_t sz, int x)
{
	return frame_trace(dst, sz, x, 1);
}

size_t API xapi_trace_pithy(char * const dst, const size_t sz)
{
	size_t z = 0;

	z += xapi_frame_error(dst + z, sz - z, callstack.l - 1);
	z += snprintf(dst + z, sz - z, " ");
	z += frame_trace(dst + z, sz - z, callstack.l - 1, 0);

	size_t zt = z;

	int x;
	for(x = callstack.l - 1; x >= 0; x--)
	{
		int y;
		for(y = 0; y < callstack.v[x]->info.l; y++)
		{
			if(callstack.v[x]->info.v[y].imp)
			{
				if(z == zt)
					z += snprintf(dst + z, sz - z, " with ");
				else
					SAY(", ");

				SAY("%.*s=%.*s"
					, callstack.v[x]->info.v[y].kl
					, callstack.v[x]->info.v[y].ks
					, callstack.v[x]->info.v[y].vl
					, callstack.v[x]->info.v[y].vs
				);
			}
		}
	}

	return z;
}

size_t API xapi_trace_full(char * const dst, const size_t sz)
{
	size_t z = 0;
	z += xapi_frame_error(dst + z, sz - z, callstack.l - 1);
	z += snprintf(dst + z, sz - z, "\n");

	int x;
	for(x = callstack.l - 1; x >= 0; x--)
	{
		z += snprintf(dst + z, sz - z, " %d : ", x);
		z += xapi_frame_trace(dst + z, sz - z, x);

		if(x)
			z += snprintf(dst + z, sz - z, "\n");
	}

	return z;
}

void API xapi_pithytrace()
{
	char space[512];

	size_t z = xapi_trace_pithy(space, sizeof(space));
	dprintf(2, "%.*s\n", (int)z, space);
}

void API xapi_fulltrace()
{
	char space[512];

	size_t z = xapi_trace_full(space, sizeof(space));
	dprintf(2, "%.*s\n", (int)z, space);
}

void API xapi_backtrace()
{
	char space[512];

	size_t z = xapi_trace_full(space, sizeof(space));
	dprintf(2, "%.*s\n", (int)z, space);
}
