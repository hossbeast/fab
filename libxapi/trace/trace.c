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
#include <inttypes.h>

#include "internal.h"
#include "errtab/XAPI.errtab.h"
#include "trace.internal.h"
#include "frame.internal.h"
#include "info.internal.h"
#include "calltree.internal.h"
#include "error.internal.h"

#include "macros.h"
#include "strutil.h"

#define restrict __restrict

#define SAY(...) z += znloadf(dst + z, sz - z, __VA_ARGS__)

//
// static
//

static size_t error_trace(char * const dst, const size_t sz, const error * const restrict e)
{
	size_t z = 0;

  if(0)
  {

  }
#if XAPI_RUNTIME_CHECKS
  else if(e->etab == perrtab_XAPI && e->code == XAPI_ILLFAIL)
  {
		SAY("[%s:%s] %s"
			, e->etab->name
			, e->code > e->etab->max ? "UNKNWN" : e->etab->v[e->code + (e->etab->min * -1)].name
			, e->code > e->etab->max ? "unspecified error" : e->etab->v[e->code + (e->etab->min * -1)].desc
		);
  }
#endif
	else if(e->etab && e->code && e->msg)
	{
		SAY("[%s:%s] %.*s"
			, e->etab->name
			, e->code > e->etab->max ? "UNKNWN" : e->etab->v[e->code + (e->etab->min * -1)].name
			, (int)e->msgl, e->msg 
		);
	}
	else if(e->etab && e->code)
	{
		SAY("[%s:%s] %s"
			, e->etab->name
			, e->code > e->etab->max ? "UNKNWN" : e->etab->v[e->code + (e->etab->min * -1)].name
			, e->code > e->etab->max ? "unspecified error" : e->etab->v[e->code + (e->etab->min * -1)].desc
		);
	}
	else if(e->etab && e->msg)
	{
		SAY("[%s] %.*s"
			, e->etab->name
			, (int)e->msgl, e->msg 
		);
	}
	else if(e->code && e->msg)
	{
		SAY("[%d] %.*s"
			, e->code
			, (int)e->msgl, e->msg 
		);
	}
	else if(e->code)
	{
		SAY("[%d]", e->code);
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

static size_t frame_trace(char * const dst, const size_t sz, frame * f, int loc, int in, int level)
{
	size_t z = 0;

	if(in)
		SAY("in ");

	z += frame_function(dst + z, sz - z, f);
	if(f->infos.l)
	{
		SAY("(");
		z += frame_info(dst + z, sz - z, f);
		SAY(")");
	}

	if(loc && f->file)
	{
		SAY(" at ");
		z += frame_location(dst + z, sz - z, f);
	}

	return z;
}

static size_t trace_frames(char * const dst, const size_t sz, calltree * const restrict ct, int x, int y, int level);

static size_t trace_frames(char * const dst, const size_t sz, calltree * const restrict ct, int x, int y, int level)
{
// k : iterator variable
// x : start index
// y : end index
// i : 
// j : 

  int k;
	size_t z = 0;

  SAY("%*s", level * 2, "");
	z += error_trace(dst + z, sz - z, ct->frames.v[x].error);
	SAY("\n");

  // first parent index which is less than the current index
  int i;
  int j = y;
  for(i = y; i > x; i--)
  for(k = y; k > x; k--)
  {
    if(ct->frames.v[k].parent_index > x)
    {
printf("@%d\n", k);
      j = ct->frames.v[k].parent_index;
      break;
    }
  }

  i = k;
  int rank = (j - x + 1) + (y - i);
printf("[x:%d, j:%d, y:%d, i:%d] %d # %d\n", x, j, y, i, level, rank);

  for(; x <= j; x++)
  {
    SAY("%*s", level * 2, "");
    SAY(" %2d/%d : ", j - x + (y - i), rank);
    z += frame_trace(dst + z, sz - z, &ct->frames.v[x], 1, 1, level);

    if((x + 1) <= j)
      SAY("\n");
  }

  if(j != y)
  {
    SAY("\n");
    z += trace_frames(dst + z, sz - z, ct, j + 1, i, level + 1);

    for(x = i + 1; x <= y; x++)
    {
      SAY("\n");
      SAY("%*s", level * 2, "");
      SAY(" %2d/%d : ", y - x, rank);
      z += frame_trace(dst + z, sz - z, &ct->frames.v[x], 1, 1, level);
    }
  }

	return z;
}

static size_t calltree_trace(char * const dst, const size_t sz, calltree * const restrict ct)
{
/*
  printf("{ \"exit_value\" : %u, \"exit_code\" : %u, \"exit_table\" : %p\n"
    , ct->exit_value
    , ct->exit_code
    , ct->exit_table
  );

  printf(", frames : [");

  int x;
  for(x = 0; x < ct->frames.l; x++)
  {
    frame * f = &ct->frames.v[x];

    if(x)
      printf(",");
    printf("{");
    printf("  \"%s\" : %ld\n", "parent_index", f->parent_index);
    printf(", \"%s\" : %p\n", "error", f->error);
    printf(", \"%s\" : \"%.*s\"\n", "file", (int)f->filel, f->file);
    printf(", \"%s\" : \"%.*s\"\n", "func", (int)f->funcl, f->func);
    printf(", \"%s\" : %d\n", "line", f->line);
    printf("}\n");
  }

  printf("]}\n");
*/

  return trace_frames(dst, sz, ct, 0, ct->frames.l - 1, 0);
}

static size_t calltree_trace_pithy(calltree * const restrict ct, char * const dst, const size_t sz)
{
	size_t z = 0;

	z += error_trace(dst + z, sz - z, ct->frames.v[0].error);

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

static size_t calltree_trace_full(calltree * const restrict ct, char * const dst, const size_t sz)
{
  return calltree_trace(dst, sz, ct);
}

//
// API
//

API size_t xapi_trace_calltree_pithy(calltree * const restrict ct, char * const restrict dst, const size_t sz)
{
  return calltree_trace_pithy(ct, dst, sz);
}

API size_t xapi_trace_calltree_full(calltree * const restrict ct, char * const restrict dst, const size_t sz)
{
	return calltree_trace_full(ct, dst, sz);
}

API size_t xapi_trace_pithy(char * const dst, const size_t sz)
{
	return calltree_trace_pithy(g_calltree, dst, sz);
}

API size_t xapi_trace_full(char * const dst, const size_t sz)
{
	return calltree_trace_full(g_calltree, dst, sz);
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
