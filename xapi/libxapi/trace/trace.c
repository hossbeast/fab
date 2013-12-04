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

static size_t frame_function(char * const dst, const size_t sz, struct frame * f)
{
	size_t z = 0;
	SAY("%s", f->func);

	return z;
}

static size_t frame_error(char * const dst, const size_t sz, struct frame * f)
{
	size_t z = 0;

	if(f->etab && f->code > 0 && f->msg)
	{
		SAY("[%s:%s] %.*s"
			, f->etab->tag
			, f->etab->v[f->code].name
			, f->msgl, f->msg 
		);
	}
	else if(f->etab && f->code > 0)
	{
		SAY("[%s:%s] %s"
			, f->etab->tag
			, f->etab->v[f->code].name
			, f->etab->v[f->code].desc
		);
	}
	else if(f->etab && f->msg)
	{
		SAY("[%s] %.*s"
			, f->etab->tag
			, f->msgl, f->msg 
		);
	}
	else if(f->code > 0 && f->msg)
	{
		SAY("[%d] %.*s"
			, f->code
			, f->msgl, f->msg 
		);
	}
	else if(f->code > 0)
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
	SAY("%s:%d", f->file, f->line);

	return z;
}

static size_t frame_trace(char * const dst, const size_t sz, struct frame * f, int loc)
{
	size_t z = 0;

	z += snprintf(dst + z, sz - z, "at ");
	z += frame_function(dst + z, sz - z, f);
	if(f->info.l)
	{
		z += snprintf(dst + z, sz - z, "(");
		z += frame_info(dst + z, sz - z, f);
		z += snprintf(dst + z, sz - z, ")");
	}

	if(loc && f->file)
	{
		z += snprintf(dst + z, sz - z, " in ");
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

int API xapi_frame_count_alt()
{
	return callstack.alt_len;
}

size_t API xapi_frame_error(char * const dst, const size_t sz, int x)
{
	return frame_error(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_error_alt(char * const dst, const size_t sz, int x)
{
	return frame_error(dst, sz, &callstack.frames.alt[x]);
}

size_t API xapi_frame_function(char * const dst, const size_t sz, int x)
{
	return frame_function(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_function_alt(char * const dst, const size_t sz, int x)
{
	return frame_function(dst, sz, &callstack.frames.alt[x]);
}

size_t API xapi_frame_location(char * const dst, const size_t sz, int x)
{
	return frame_location(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_location_alt(char * const dst, const size_t sz, int x)
{
	return frame_location(dst, sz, &callstack.frames.alt[x]);
}

size_t API xapi_frame_info(char * const dst, const size_t sz, int x)
{
	return frame_info(dst, sz, callstack.v[x]);
}

size_t API xapi_frame_trace(char * const dst, const size_t sz, int x)
{
	return frame_trace(dst, sz, callstack.v[x], 1);
}

size_t API xapi_frame_trace_alt(char * const dst, const size_t sz, int x)
{
	return frame_trace(dst, sz, &callstack.frames.alt[x], 1);
}

size_t API xapi_trace_pithy(char * const dst, const size_t sz)
{
	size_t z = 0;

	// alt stack
	int x;
	if(callstack.alt_len)
	{
		z += frame_error(dst + z, sz - z, &callstack.frames.alt[0]);
		z += snprintf(dst + z, sz - z, " ");
		z += frame_trace(dst + z, sz - z, &callstack.frames.alt[0], 0);
	}

	// norm stack
	if(callstack.v[callstack.l - 1]->code > 0)
	{
		if(callstack.alt_len)
			z += snprintf(dst + z, sz - z, " after ");

		z += frame_error(dst + z, sz - z, callstack.v[callstack.l - 1]);
		z += snprintf(dst + z, sz - z, " ");
		z += frame_trace(dst + z, sz - z, callstack.v[callstack.l - 1], 0);
	}

	size_t zt = z;

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

typeof(callstack) * T;

size_t API xapi_trace_full(char * const dst, const size_t sz)
{
	size_t z = 0;

T = &callstack;

	// alt stack
	int x;
	if(callstack.alt_len)
	{
		z += frame_error(dst + z, sz - z, &callstack.frames.alt[0]);

		for(x = 0; x < callstack.alt_len; x++)
		{
			z += snprintf(dst + z, sz - z, "\n");
			z += snprintf(dst + z, sz - z, " %d : ", x);
			z += frame_trace(dst + z, sz - z, &callstack.frames.alt[x], 1);
		}
	}

	// norm stack
	if(z)
		z += snprintf(dst + z, sz - z, "\n");
	z += frame_error(dst + z, sz - z, callstack.v[callstack.l - 1]);
	z += snprintf(dst + z, sz - z, "\n");

	for(x = callstack.l - 1; x >= 0; x--)
	{
		if(x != callstack.l - 1)
			z += snprintf(dst + z, sz - z, "\n");

		z += snprintf(dst + z, sz - z, " %d : ", x);
		z += frame_trace(dst + z, sz - z, callstack.v[x], 1);
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
