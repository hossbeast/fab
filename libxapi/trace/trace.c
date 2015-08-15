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

#include <stdio.h>
#include <string.h>

#include "internal.h"
#include "trace.internal.h"
#include "frame.internal.h"
#include "info.h"
#include "stack.h"
#include "calltree.internal.h"

#include "macros.h"
#include "strutil.h"

#define restrict __restrict

//
// static
//

#define SAY(...) z += znloadf(dst + z, sz - z, __VA_ARGS__)

static size_t stack_error(char * const dst, const size_t sz, stack * s)
{
	size_t z = 0;

	if(s->etab && s->code && s->msg)
	{
		SAY("[%s:%s] %.*s"
			, s->etab->tag
			, s->code > s->etab->max ? "UNKNWN" : s->etab->v[s->code + (s->etab->min * -1)].name
			, (int)s->msgl, s->msg 
		);
	}
	else if(s->etab && s->code)
	{
		SAY("[%s:%s] %s"
			, s->etab->tag
			, s->code > s->etab->max ? "UNKNWN" : s->etab->v[s->code + (s->etab->min * -1)].name
			, s->code > s->etab->max ? "unspecified error" : s->etab->v[s->code + (s->etab->min * -1)].desc
		);
	}
	else if(s->etab && s->msg)
	{
		SAY("[%s] %.*s"
			, s->etab->tag
			, (int)s->msgl, s->msg 
		);
	}
	else if(s->code && s->msg)
	{
		SAY("[%d] %.*s"
			, s->code
			, (int)s->msgl, s->msg 
		);
	}
	else if(s->code)
	{
		SAY("[%d]", s->code);
	}

	return z;
}

static size_t frame_function(char * const dst, const size_t sz, frame * f)
{
	size_t z = 0;
	SAY("%s", f->func);

	return z;
}

static size_t frame_info(char * const dst, const size_t sz, frame * f)
{
	size_t z = 0;

	int y;
	for(y = 0; y < f->infos.l; y++)
	{
		if(y)
			SAY(", ");

		SAY("%.*s=%.*s"
			, (int)f->infos.v[y].kl
			, f->infos.v[y].ks
			, (int)f->infos.v[y].vl
			, f->infos.v[y].vs
		);
	}

	return z;
}

static size_t frame_location(char * const dst, const size_t sz, frame * f)
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

static size_t stack_trace(char * const dst, const size_t sz, stack * s, int level);

static size_t frame_trace(char * const dst, const size_t sz, frame * f, int loc, int at, int level)
{
	size_t z = 0;

 // SAY("%*s", level * 2, "");

	if(at)
		SAY("at ");

	z += frame_function(dst + z, sz - z, f);
	if(f->infos.l)
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

  if(f->child)
  {
    SAY("(%zu)\n", f->child->frames.l);
    z += stack_trace(dst + z, sz - z, f->child, level + 1);
  }

	return z;
}

static size_t stack_trace(char * const dst, const size_t sz, stack * s, int level)
{
	size_t z = 0;

  SAY("%*s", level * 2, "");
	z += stack_error(dst + z, sz - z, s);
	SAY("\n");

	int x;
  for(x = 0; x < s->frames.l; x++)
	{
		if(x)
			SAY("\n");

    SAY("%*s", level * 2, "");
		SAY(" %2d : ", (int)s->frames.l - x - 1);
		z += frame_trace(dst + z, sz - z, &s->frames.v[x], 1, 1, level);
	}

	return z;
}

static size_t calltree_trace_pithy(stack * const restrict ct, char * const dst, const size_t sz)
{
	size_t z = 0;

	z += stack_error(dst + z, sz - z, ct);

	info * nfo = 0;

	size_t zt = z;
	int x;
//	for(x = ct->frames.l; x >= MAX(ct->frames.l - 5 /* heuristic */, 0); x--)
  for(x = 0; x < ct->frames.l; x++)
	{
		int y;
		for(y = 0; y < ct->frames.v[x].infos.l; y++)
		{
			// determine whether an info by this name has already been used
			int xx;
			for(xx = x + 1; xx < ct->frames.l; xx++)
			{
				int yy;
				for(yy = 0; yy < ct->frames.v[xx].infos.l; yy++)
				{
					if(estrcmp(
							ct->frames.v[x].infos.v[y].ks
						, ct->frames.v[x].infos.v[y].kl
						, ct->frames.v[xx].infos.v[yy].ks
						, ct->frames.v[xx].infos.v[yy].kl
						, 0) == 0)
					{
						break;
					}
				}
				if(yy < ct->frames.v[xx].infos.l)
				{
					break;
				}
			}

			if(xx == ct->frames.l)
			{
				if(nfo)
				{
					if(z == zt)
						SAY(" with ");
					else
						SAY(", ");

					SAY("%.*s=%.*s"
						, (int)nfo->kl
						, nfo->ks
						, (int)nfo->vl
						, nfo->vs
					);
				}
				nfo = &ct->frames.v[x].infos.v[y];
			}
		}
	}

	if(nfo)
	{
		if(z == zt)
			SAY(" with ");
		else
			SAY(" and ");

		SAY("%.*s=%.*s"
			, (int)nfo->kl
			, nfo->ks
			, (int)nfo->vl
			, nfo->vs
		);
	}

	return z;
}

static size_t calltree_trace_full(stack * const restrict ct, char * const dst, const size_t sz)
{
  return stack_trace(dst, sz, ct, 0);
}

//
// API
//

API size_t xapi_trace_calltree_pithy(stack * const restrict cs, char * const restrict dst, const size_t sz)
{
  return calltree_trace_pithy(cs, dst, sz);
}

API size_t xapi_trace_calltree_full(stack * const restrict cs, char * const restrict dst, const size_t sz)
{
	return calltree_trace_full(cs, dst, sz);
}

API size_t xapi_trace_pithy(char * const dst, const size_t sz)
{
	return calltree_trace_pithy(g_stack, dst, sz);
}

API size_t xapi_trace_full(char * const dst, const size_t sz)
{
	return calltree_trace_full(g_stack, dst, sz);
}

API void xapi_pithytrace()
{
	char space[512];

	size_t z = xapi_trace_pithy(space, sizeof(space));
	dprintf(2, "%.*s\n", (int)z, space);
}

API void xapi_fulltrace()
{
	char space[512];

	size_t z = xapi_trace_full(space, sizeof(space));
	dprintf(2, "%.*s\n", (int)z, space);
}

API void xapi_backtrace()
{
	char space[512];

	size_t z = xapi_trace_full(space, sizeof(space));
	dprintf(2, "%.*s\n", (int)z, space);
}
