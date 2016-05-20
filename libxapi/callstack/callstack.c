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
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#include "memblk.def.h"

typedef struct memblk memblk;

#define restrict __restrict

//
// static
//
static void frame_freeze(memblk * const restrict mb, struct frame * f)
{
	/*
	** etab is statically allocated and must be handled specially
	*/
	f->etab = (void*)(intptr_t)f->etab->id;
	memblk_freeze(mb, &f->file);
	memblk_freeze(mb, &f->func);
	memblk_freeze(mb, &f->msg);

	int x;
	for(x = 0; x < f->info.l; x++)
	{
		memblk_freeze(mb, &f->info.v[x].ks);
		memblk_freeze(mb, &f->info.v[x].vs);
	}
	memblk_freeze(mb, &f->info.v);
}

static void frame_unfreeze(memblk * const restrict mb, struct frame * f)
{
	f->etab = xapi_errtab_byid((intptr_t)f->etab);
	memblk_unfreeze(mb, &f->file);
	memblk_unfreeze(mb, &f->func);
	memblk_unfreeze(mb, &f->msg);

	memblk_unfreeze(mb, &f->info.v);

	int x;
	for(x = 0; x < f->info.l; x++)
	{
		memblk_unfreeze(mb, &f->info.v[x].ks);
		memblk_unfreeze(mb, &f->info.v[x].vs);
	}
}

static void frame_thaw(char * const restrict mb, struct frame * f)
{
	f->etab = xapi_errtab_byid((intptr_t)f->etab);
	memblk_thaw(mb, &f->file);
	memblk_thaw(mb, &f->func);
	memblk_thaw(mb, &f->msg);

	memblk_thaw(mb, &f->info.v);

	int x;
	for(x = 0; x < f->info.l; x++)
	{
		memblk_thaw(mb, &f->info.v[x].ks);
		memblk_thaw(mb, &f->info.v[x].vs);
	}
}

//
// public
//
void callstack_free()
{
	int x;
	for(x = 0; x < callstack_mb.blocksl; x++)
	{
		free(callstack_mb.blocks[x].s);
		memset(&callstack_mb.blocks[x], 0, sizeof(callstack_mb.blocks[0]));
	}
	free(callstack_mb.blocks);
	memset(&callstack_mb, 0, sizeof(callstack_mb));

	callstack = 0;

#if DEVEL
	CS = callstack;
#endif
}

//
// api
//
API void xapi_callstack_unwindto(int frame)
{
	// x tracks the position when unwinding
	callstack->x = -1;

	// reset frame pointer to specified depth
	callstack->l = frame;

	callstack->v[callstack->l - 1]->code = 0;
}

API memblk * xapi_callstack_freeze()
{
	memblk * const mb = &callstack_mb;

	int x;
	for(x = 0; x < callstack->l; x++)
	{
		frame_freeze(mb, callstack->frames.stor[x]);
		memblk_freeze(mb, &callstack->frames.stor[x]);
		memblk_freeze(mb, &callstack->v[x]);
	}

	memblk_freeze(mb, &callstack->frames.stor);
	memblk_freeze(mb, &callstack->v);

	return mb;
}

API void xapi_callstack_unfreeze()
{
	memblk * const mb = &callstack_mb;
	struct callstack * const cs = callstack;

	memblk_unfreeze(mb, &cs->v);
	memblk_unfreeze(mb, &cs->frames.stor);

	int x;
	for(x = 0; x < cs->l; x++)
	{
		memblk_unfreeze(mb, &cs->v[x]);
		memblk_unfreeze(mb, &cs->frames.stor[x]);
		frame_unfreeze(mb, cs->frames.stor[x]);
	}
}

API struct callstack * xapi_callstack_thaw(char * const restrict mb)
{
	struct callstack * cs = (void*)mb;
	memblk_thaw(mb, &cs->v);
	memblk_thaw(mb, &cs->frames.stor);

	int x;
	for(x = 0; x < cs->l; x++)
	{
		memblk_thaw(mb, &cs->v[x]);
		memblk_thaw(mb, &cs->frames.stor[x]);
		frame_thaw(mb, cs->frames.stor[x]);
	}

	return cs;
}

API size_t xapi_callstack_trace_pithy(struct callstack * const restrict cs, char * const restrict dst, const size_t sz)
{
	return callstack_trace_pithy(cs, dst, sz);
}

API size_t xapi_callstack_trace_full(struct callstack * const restrict cs, char * const restrict dst, const size_t sz)
{
	return callstack_trace_full(cs, dst, sz);
}
