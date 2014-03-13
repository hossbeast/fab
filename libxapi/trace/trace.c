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

#include "internal.h"

#include "macros.h"
#include "strutil.h"

///
/// backtrace generation
///

#define SAY(...) do {																\
		if(sz - z)																			\
			z += snprintf(dst + z, sz - z, __VA_ARGS__);	\
	} while(0)

static size_t frame_function(char * const dst, const size_t sz, struct frame * f)
{
	size_t z = 0;
	SAY("%s", f->func);

	return z;
}

static size_t frame_error(char * const dst, const size_t sz, struct frame * f)
{
	size_t z = 0;

	if(f->etab && f->code && f->msg)
	{
		SAY("[%s:%s] %.*s"
			, f->etab->tag
			, f->code > f->etab->max ? "UNKNWN" : f->etab->v[f->code + (f->etab->min * -1)].name
			, f->msgl, f->msg 
		);
	}
	else if(f->etab && f->code)
	{
		SAY("[%s:%s] %s"
			, f->etab->tag
			, f->code > f->etab->max ? "UNKNWN" : f->etab->v[f->code + (f->etab->min * -1)].name
			, f->code > f->etab->max ? "unspecified error" : f->etab->v[f->code + (f->etab->min * -1)].desc
		);
	}
	else if(f->etab && f->msg)
	{
		SAY("[%s] %.*s"
			, f->etab->tag
			, f->msgl, f->msg 
		);
	}
	else if(f->code && f->msg)
	{
		SAY("[%d] %.*s"
			, f->code
			, f->msgl, f->msg 
		);
	}
	else if(f->code)
	{
		SAY("[%d]", f->code);
	}

	return z;
}

static size_t frame_info(char * const dst, const size_t sz, struct frame * f)
{
	size_t z = 0;

	int y;
	for(y = 0; y < f->info.l; y++)
	{
		if(y)
			SAY(", ");

		SAY("%.*s=%.*s"
			, f->info.v[y].kl
			, f->info.v[y].ks
			, f->info.v[y].vl
			, f->info.v[y].vs
		);
	}

	return z;
}

static size_t frame_location(char * const dst, const size_t sz, struct frame * f)
{
	size_t z = 0;

	const char * file = f->file;
	const char * n;
	while((n = strstr(file, "/")))
	{
		file = n + 1;
	}

	SAY("%s:%d", file, f->line);

	return z;
}

static size_t frame_trace(char * const dst, const size_t sz, struct frame * f, int loc, int at)
{
	size_t z = 0;

	if(at)
		SAY("at ");

	z += frame_function(dst + z, sz - z, f);
	if(f->info.l)
	{
		SAY("(");
		z += frame_info(dst + z, sz - z, f);
		SAY(")");
	}

	if(loc && f->file)
	{
		SAY(" in ");
		z += frame_location(dst + z, sz - z, f);
	}

	return z;
}

///
/// API
///

int API xapi_frame_count()
{
	return callstack.l;
}

size_t API xapi_frame_error(char * const dst, const size_t sz, int x)
{
	return frame_error(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_function(char * const dst, const size_t sz, int x)
{
	return frame_function(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_location(char * const dst, const size_t sz, int x)
{
	return frame_location(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_infostring(char * const dst, const size_t sz, int x)
{
	return frame_info(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_trace(char * const dst, const size_t sz, int x)
{
	return frame_trace(dst, sz, callstack.v[x], 1, 1);
}

size_t API xapi_trace_pithy(char * const dst, const size_t sz)
{
	size_t z = 0;

	z += frame_error(dst + z, sz - z, callstack.v[callstack.l - 1]);
	SAY(" ");
//	z += frame_trace(dst + z, sz - z, callstack.v[callstack.l - 1], 0, callstack.v[callstack.l - 1]->code);

	size_t zt = z;
	int x;
	for(x = callstack.l; x >= MAX(callstack.l - 5 /* heuristic */, 0); x--)
	{
		int y;
		for(y = 0; y < callstack.v[x]->info.l; y++)
		{
			// determine whether an info by this name has already been used
			int xx;
			for(xx = x + 1; xx < callstack.l; xx++)
			{
				int yy;
				for(yy = 0; yy < callstack.v[xx]->info.l; yy++)
				{
					if(estrcmp(
							callstack.v[x]->info.v[y].ks
						, callstack.v[x]->info.v[y].kl
						, callstack.v[xx]->info.v[yy].ks
						, callstack.v[xx]->info.v[yy].kl
						, 0) == 0)
					{
						break;
					}
				}
				if(yy < callstack.v[xx]->info.l)
				{
					break;
				}
			}

			if(xx == callstack.l)
			{
				if(z == zt)
					SAY("with ");
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

	z += frame_error(dst + z, sz - z, callstack.v[callstack.l - 1]);
	SAY("\n");

	int x;
	for(x = callstack.l - 1; x >= 0; x--)
	{
		if(x != callstack.l - 1)
			SAY("\n");

		SAY(" %2d : ", x);
		z += frame_trace(dst + z, sz - z, callstack.v[x], 1, 1);
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
