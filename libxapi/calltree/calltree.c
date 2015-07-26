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
#include "calltree.h"
#include "calltree.internal.h"

#include "memblk.def.h"

// per-thread calltree storage
__thread struct calltree * calltree;
__thread struct memblk callstack_mb;

#define restrict __restrict

//
// static
//
static void frame_freeze(memblk * const restrict mb, struct frame * f)
{
	stack_freeze(mb, f->exe);

	memblk_freeze(mb, &f->file);
	memblk_freeze(mb, &f->func);
	memblk_freeze(mb, &f->exe);
}

static void frame_unfreeze(memblk * const restrict mb, struct frame * f)
{
	memblk_unfreeze(mb, &f->file);
	memblk_unfreeze(mb, &f->func);
  memblk_unfreeze(mb, &f->exe);

  stack_unfreeze(mb, f->exe);
}

static void frame_thaw(char * const restrict mb, struct frame * f)
{
	memblk_thaw(mb, &f->file);
	memblk_thaw(mb, &f->func);
	memblk_thaw(mb, &f->exe);

  stack_thaw(mb, f->exe);
}

static void info_freeze(memblk * const restrict mb, struct info * i)
{
  memblk_freeze(mb, &i->ks);
  memblk_freeze(mb, &i->vs);
}

static void info_unfreeze(memblk * const restrict mb, struct info * i)
{
  memblk_unfreeze(mb, &i->ks);
  memblk_unfreeze(mb, &i->vs);
}

static void info_thaw(char * const restrict mb, struct info * i)
{
  memblk_thaw(mb, &i->ks);
  memblk_thaw(mb, &i->vs);
}

static void stack_freeze(memblk * const restrict mb, struct stack * s)
{
	/*
	** etab is statically allocated and must be handled specially
	*/
	s->etab = (void*)(intptr_t)s->etab->id;
	memblk_freeze(mb, &s->msg);
	memblk_freeze(mb, &s->parent);

	int x;
	for(x = 0; x < s->info.l; x++)
		info_freeze(mb, &s->info.v[x]);
	memblk_freeze(mb, &s->info.v);

	for(x = 0; x < s->frames.l; x++)
		frame_freeze(mb, &s->frames.v[x]);
  memblk_freeze(mb, &s->frames.v);
}

static void stack_unfreeze(memblk * const restrict mb, struct stack * s)
{
	s->etab = xapi_errtab_byid((intptr_t)s->etab);
	memblk_unfreeze(mb, &s->msg);
	memblk_unfreeze(mb, &s->parent);

	memblk_unfreeze(mb, &s->info.v);
	int x;
	for(x = 0; x < s->info.l; x++)
		info_unfreeze(mb, &s->info.v[x]);

	memblk_unfreeze(mb, &s->frames.v);
	for(x = 0; x < s->frames.l; x++)
		frame_unfreeze(mb, &s->frames.v[x]);
}

static void stack_thaw(char * const restrict mb, struct stack * s)
{
	s->etab = xapi_errtab_byid((intptr_t)s->etab);
	memblk_thaw(mb, &s->msg);
	memblk_thaw(mb, &s->parent);

	memblk_thaw(mb, &s->info.v);
	int x;
	for(x = 0; x < s->info.l; x++)
		info_thaw(mb, &s->info.v[x]);

	memblk_thaw(mb, &s->frames.v);
	for(x = 0; x < s->frames.l; x++)
		frame_thaw(mb, &s->frames.v[x]);
}

//
// public
//

void wmalloc(void * p, size_t sz)
{
#define MEMBLOCK_SMALL			0x1000 /* 1k : first THRESHOLD blocks */
#define MEMBLOCK_THRESHOLD	0x4			
#define MEMBLOCK_LARGE			0x8000 /* 8k : additional blocks */

	// request is too large to satisfy
	if(sz > MEMBLOCK_LARGE)
		goto failed;

	struct memblk * mb = &callstack_mb;

	/* current block is full */
	if(mb->blocksl == 0 || ((mb->blocks[mb->blocksl - 1].l + sz) > mb->blocks[mb->blocksl - 1].a))
	{
		/* reallocate the block container */
		if(mb->blocksl == mb->blocksa)
		{
			size_t ns = mb->blocksa ?: 3;
			ns = ns * 2 + ns / 2;
			if((mb->blocks = calloc(sizeof(*mb->blocks) * ns, 1)) == 0)
				goto failed;
			mb->blocksa = ns;
		}

		if((mb->blocksl < MEMBLOCK_THRESHOLD) && (sz <= MEMBLOCK_SMALL))
			mb->blocks[mb->blocksl].a = MEMBLOCK_SMALL;
		else
			mb->blocks[mb->blocksl].a = MEMBLOCK_LARGE;

		if((mb->blocks[mb->blocksl].s = calloc(sizeof(*mb->blocks[0].s) * mb->blocks[mb->blocksl].a, 1)) == 0)
			goto failed;

		// cumulative offset
		if(mb->blocksl)
			mb->blocks[mb->blocksl].o = mb->blocks[mb->blocksl - 1].o + mb->blocks[mb->blocksl - 1].l;

		mb->blocksl++;
	}

	*(void**)p = mb->blocks[mb->blocksl - 1].s + mb->blocks[mb->blocksl - 1].l;
	mb->blocks[mb->blocksl - 1].l += sz;

	return;

failed:
  write(2, "libxapi failure to allocate while unwinding ; consider calling xapi_preallocate\n");
}

void wrealloc(void * p, size_t es, size_t ec, size_t oec)
{
	void * old = *(void**)p;
	wmalloc(p, es * ec);
	if(old)
		memcpy(*(void**)p, old, es * oec);
}

void scatw(char ** dst, size_t * dstl, size_t * dsta, size_t seed, const char * s, size_t l)
{
	if(l >= (*dsta))
	{
		size_t ns = (*dsta) ?: seed;
		while(ns <= l)
			ns = ns * 2 + ns / 2;
		wrealloc(dst, 1, ns, *dsta);
		(*dsta) = ns;
	}

	(*dstl) = l;
	memcpy(*dst, s, l);
	(*dst)[l] = 0;
}

void calltree_free()
{
	int x;
	for(x = 0; x < calltree_mb.blocksl; x++)
	{
		free(calltree_mb.blocks[x].s);
		memset(&calltree_mb.blocks[x], 0, sizeof(calltree_mb.blocks[0]));
	}
	free(calltree_mb.blocks);
	memset(&calltree_mb, 0, sizeof(calltree_mb));

	calltree = 0;

#if DEVEL
	CS = calltree;
#endif
}

//
// api
//
API void xapi_calltree_unwindto(int frame)
{
	// x tracks the position when unwinding
	calltree->x = -1;

	// reset frame pointer to specified depth
	calltree->l = frame;

	calltree->v[calltree->l - 1]->code = 0;
}

API memblk * xapi_calltree_freeze()
{
	memblk * const mb = &calltree_mb;
  struct calltree * const cs = calltree;

  // freze the root stack ; the exe stack is in its hierarchy
  stack_freeze(mb, cs->root);

  memblk_freeze(mb, &cs->exe);
  memblk_freeze(mb, &cs->root);

  return mb;
}

API void xapi_calltree_unfreeze()
{
	memblk * const mb = &calltree_mb;
	struct calltree * const cs = calltree;

  memblk_unfreeze(mb, &cs->exe);
  memblk_unfreeze(mb, &cs->root);

  stack_unfreeze(mb, cs->root);
}

API struct calltree * xapi_calltree_thaw(char * const restrict mb)
{
	struct calltree * cs = (void*)mb;
  memblk_thaw(mb, &cs->exe);
  memblk_thaw(mb, &cs->root);

  stack_thaw(mb, cs->root);

  return cs;
}

API size_t xapi_calltree_trace_pithy(struct calltree * const restrict cs, char * const restrict dst, const size_t sz)
{
	return calltree_trace_pithy(cs, dst, sz);
}

API size_t xapi_calltree_trace_full(struct calltree * const restrict cs, char * const restrict dst, const size_t sz)
{
	return calltree_trace_full(cs, dst, sz);
}

API int xapi_calltree_allocate(int frames)
{
  // calculate the new frame lead
  calltree->frame_lead = xapi_frame_lead + (calltree->l * xapi_frame_lead_factor);

  // new frame allocation
  size_t ns = calltree->frames.a;
  size_t es = calltree->frames.a + calltree->frame_lead + 1;
  while(ns <= es)
    ns = ns * 2 + ns / 2;
  fatal(xrealloc, &calltree->frames.v, sizeof(*calltree->frames.v), ns, calltree->frames.a);
  calltree->frames.a = ns;

  // schedule the next allocation
  calltree->scheduled_allocation = calltree->frames.a - calltree->frame_lead;

  // stacks is proportional to frames
  ns = calltree->stacks.a;
  es = 2 + (calltree->frames.a * xapi_stacks_factor);
  while(ns <= es)
    ns = ns * 2 + ns / 2;
  fatal(xrealloc, &calltree->stacks.v, sizeof(*calltree->stacks.v), ns, calltree->stacks.a);
  calltree->stacks.a = ns;

  // infos is proportional to stacks
  ns = calltree->infos.a;
  es = calltree->stacks.a * xapi_infos_factor;
  while(ns <= es)
    ns = ns * 2 + ns / 2;
  fatal(xrealloc, &calltree->infos.v, sizeof(*calltree->infos.v), ns, calltree->infos.a);
  calltree->infos.a = ns;

  // strings is proportional to infos
  //  file + func + msg + (k/v per info)
  ns = calltree->strings.a;
  es = (calltree->infos.a * (1 + 1 + 1 + (calltree->infos.a * 2))) * xapi_strings_factor;
  while(ns <= es)
    ns = ns * 2 + ns / 2;
  fatal(xrealloc, &calltree->strings.v, sizeof(*calltree->strings.v), ns, calltree->strings.a);
  calltree->strings.a = ns;
}
