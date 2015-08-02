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
#include <string.h>
#include <stdio.h>

#include "internal.h"
#include "frame.internal.h"
#include "calltree.internal.h"
#include "stack.h"
#include "mm.internal.h"
#include "info.h"

#include "memblk.def.h"
#include "macros.h"

#define restrict __restrict

#if XAPI_RUNTIME_CHECKS
__thread struct calltree_frames g_calltree_frames;
#endif

/// frame_set
//
// SUMMARY
//
//
static void frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
  // save site-of-error information on the stack
  if(g_calltree->exe->etab == 0)
  {
    g_calltree->exe->etab = etab;
    g_calltree->exe->code = code ?: etab->max + 1;	// no code given
    g_calltree->exe->msgl = 0;
  }

  // allocate a new frame
  if(g_calltree->exe->frames.l == g_calltree->exe->frames.a)
  {
    size_t ns = g_calltree->exe->frames.a ?: 10;
    ns = ns * 2 + ns / 2;
    wrealloc(&g_calltree->exe->frames.v, sizeof(*g_calltree->exe->frames.v), ns, g_calltree->exe->frames.a);
    g_calltree->exe->frames.a = ns;
  }

	frame * f = &g_calltree->exe->frames.v[g_calltree->exe->frames.l++];

  f->line = line;
  f->infos.l = 0;

  size_t funcl = strlen(func);
  size_t filel = 0;
  if(file)
    filel = strlen(file);

  sloadw(&f->file, &f->filel, &f->filea, file, filel);
  sloadw(&f->func, &f->funcl, &f->funca, func, funcl);
}

//
// public
//

void frame_freeze(memblk * const restrict mb, frame * f)
{
	memblk_freeze(mb, &f->file);
	memblk_freeze(mb, &f->func);

	int x;
	for(x = 0; x < f->infos.l; x++)
		info_freeze(mb, &f->infos.v[x]);
	memblk_freeze(mb, &f->infos.v);

	stack_freeze(mb, f->exe);
	memblk_freeze(mb, &f->exe);
}

void frame_unfreeze(memblk * const restrict mb, frame * f)
{
	memblk_unfreeze(mb, &f->file);
	memblk_unfreeze(mb, &f->func);

	memblk_unfreeze(mb, &f->infos.v);
	int x;
	for(x = 0; x < f->infos.l; x++)
		info_unfreeze(mb, &f->infos.v[x]);

  memblk_unfreeze(mb, &f->exe);
  stack_unfreeze(mb, f->exe);
}

void frame_thaw(char * const restrict mb, frame * f)
{
	memblk_thaw(mb, &f->file);
	memblk_thaw(mb, &f->func);

	memblk_thaw(mb, &f->infos.v);
	int x;
	for(x = 0; x < f->infos.l; x++)
		info_thaw(mb, &f->infos.v[x]);

	memblk_thaw(mb, &f->exe);
  stack_thaw(mb, f->exe);
}

///
/// calltree manipulation
///

#if XAPI_RUNTIME_CHECKS
int API xapi_frame_enter(void * calling_frame)
{
  assure(&g_calltree_frames.v, &g_calltree_frames.l, &g_calltree_frames.a, g_calltree_frames.l + 1);

  g_calltree_frames.v[g_calltree_frames.l++] = calling_frame;

  return g_calltree_frames.l - 1;
}

API void * xapi_frame_caller()
{
  void * f = 0;
  if(g_calltree_frames.l)
    f = g_calltree_frames.v[g_calltree_frames.l - 1];

  return f;
}

int API xapi_frame_depth()
{
  return g_calltree_frames.l;
}
#endif

int API xapi_frame_leave()
{
#if XAPI_RUNTIME_CHECKS
  g_calltree_frames.l--;   // discard the frame
#endif

  int r = 0;
  xapi_frame_leave3(0, 0, &r);
  return r;
}

void API xapi_frame_leave3(const etable ** restrict etab, int * const restrict code, int * const restrict rval)
{
#if XAPI_RUNTIME_CHECKS
  g_calltree_frames.l--;   // discard the frame
#endif

  const etable * E = 0;
  int C = 0;
  int R = 0;

  if(g_calltree && g_calltree->exe)
  {
    E = g_calltree->exe->etab;
    C = g_calltree->exe->code;
    if(E)
      R = (E->id << 16) | C;
  }

	if(etab) (*etab) = E;
	if(code) (*code) = C;
	if(rval) (*rval) = R;
}

typedef const etable * etabstar;
etabstar API xapi_frame_errtab()
{
#if DEVEL
	CT = g_calltree;
#endif

	return g_calltree->exe->etab;
}

int API xapi_frame_errcode()
{
#if DEVEL
	CT = g_calltree;
#endif

	return g_calltree->exe->code;
}

int API xapi_frame_errval()
{
#if DEVEL
	CT = g_calltree;
#endif

  if(g_calltree->exe->etab)
    return (g_calltree->exe->etab->id << 16) | g_calltree->exe->code;

	return 0;
}

int API xapi_unwinding()
{
#if DEVEL
	CT = g_calltree;
#endif

  return !!g_calltree;
}

void API xapi_frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
	frame_set(etab, code, file, line, func);
}

void API xapi_frame_set_messagew(const etable * const etab, const int16_t code, const char * const msg, int msgl, const char * const file, const int line, const char * const func)
{
#if DEVEL
	CT = g_calltree;
#endif

	frame_set(etab, code, file, line, func);
	msgl = msgl ?: msg ? strlen(msg) : 0;

  // save the msg when setting the base frame for the stack
  stack * s = g_calltree->exe;
	if(s->frames.l == 1)
    sloadw(&s->msg, &s->msgl, &s->msga, msg, msgl);
}

void API xapi_frame_set_messagef(const etable * const etab, const int16_t code, const char * const fmt, const char * const file, const int line, const char * const func, ...)
{
#if DEVEL
	CT = g_calltree;
#endif

	frame_set(etab, code, file, line, func);

  // save the msg when setting the base frame for the stack
  stack * s = g_calltree->exe;
	if(s->frames.l == 1)
  {
    va_list va;
    va_start(va, func);
    svloadf(&s->msg, &s->msgl, &s->msga, fmt, va);
    va_end(va);
  }
}

void API xapi_frame_infow(const char * const k, int kl, const char * const v, int vl)
{
#if DEVEL
	CT = g_calltree;
#endif

  if(k)
    kl = kl ?: strlen(k);
  if(v)
    vl = vl ?: strlen(v);

  if(k && kl && v && vl)
  {
    frame * f = &g_calltree->exe->frames.v[g_calltree->exe->frames.l - 1];

    // ensure allocation for the info list
    assure(&f->infos.v, &f->infos.l, &f->infos.a, f->infos.l + 1);
    info * i = &f->infos.v[f->infos.l++];

    // save the strings
    sloadw(&i->ks, &i->kl, &i->ka, k, kl);
    sloadw(&i->vs, &i->vl, &i->va, v, vl);
  }
}

void API xapi_frame_infof(const char * const k, int kl, const char * const vfmt, ...)
{
#if DEVEL
	CT = g_calltree;
#endif

  if(k)
    kl = kl ?: strlen(k);

  // measure
  va_list va;
  va_start(va, vfmt);

  va_list va2;
  va_copy(va2, va);
  size_t vl = vsnprintf(0, 0, vfmt, va2);
  va_end(va2);

  if(k && kl && vl)
  {
    frame * f = &g_calltree->exe->frames.v[g_calltree->exe->frames.l - 1];

    // ensure allocation for the info list
    assure(&f->infos.v, &f->infos.l, &f->infos.a, f->infos.l + 1);
    info * i = &f->infos.v[f->infos.l++];

    // save the strings
    sloadw(&i->ks, &i->kl, &i->ka, k, kl);
    svloadf(&i->vs, &i->vl, &i->va, vfmt, va);
  }

  va_end(va);
}
#if 0
#if XAPI_RUNTIME_CHECKS
int API xapi_frame_enter(void * calling_frame)
#else
int API xapi_frame_enter()
#endif
{
  size_t ns;
  size_t es;

	if(calltree == 0)
	{
		/* if this allocation fails, the program will segfault
		**  it WILL fail if MEMBLOCK_LARGE < sizeof(calltree)
		**  it could of course also fail if there is actually insufficient memory
		*/
		if(wmalloc(&calltree, sizeof(*calltree)))
    {
      write(2, "libxapi failed to allocate calltree");
    }
#if DEVEL
	CT = g_calltree;
#endif

    xapi_calltree_allocate(1);

    g_calltree->exe = &g_calltree->stacks.v[0];
	}

  if(g_calltree->exe->code)
  {
    g_calltree->exe = &g_calltree->stacks.v[g_calltree->stacks.l++]
  }

  else if(g_calltree->l == g_calltree->scheduled_allocation)
  {
    // calculate the new frame lead
    g_calltree->frame_lead = xapi_frame_lead + (g_calltree->l * xapi_frame_lead_factor);

    // new frame allocation
    xapi_calltree_allocate(g_calltree->frames.a + g_calltree->frame_lead + 1);    

    // schedule the next allocation
    g_calltree->scheduled_allocation = g_calltree->frames.a - g_calltree->frame_lead;
  }

#if 0
	if(g_calltree->v && g_calltree->v[g_calltree->l - 1]->code)
	{
		// unwinding is underway ; no new frame
		return g_calltree->r = -1;
	}

	// first push
	if(g_calltree->x == g_calltree->l == 0)
		g_calltree->x = -1;

//printf("ENTER :: [x=%2d][l=%2d] => ", g_calltree->x, g_calltree->l);

	// ensure frame allocation
	int ns = g_calltree->a;
	if(g_calltree->l == g_calltree->a)
	{
		ns = g_calltree->a ?: 10;
		ns = ns * 2 + ns / 2;

		/*
		** the frame list is allocated with 2 extra slots ; one for the base frame (at the beginning) and one
		** for the alt frame (at the end)
		*/
		if(wrealloc(&g_calltree->v, sizeof(*g_calltree->v), ns + 2, g_calltree->a) == 0)
		{
			if(wrealloc(&g_calltree->frames.stor, sizeof(*g_calltree->frames.stor), ns, g_calltree->a) == 0)
			{
				int x;
				for(x = g_calltree->a; x < ns; x++)
				{
					if(wrealloc(&g_calltree->frames.stor[x], sizeof(*g_calltree->frames.stor[0]), 1, 0))
						break;
				}

				g_calltree->a = x;
			}
		}
	}

	if(g_calltree->a != ns)
	{
		// if the frame list has not yet been allocated, use the static frame list
		if(g_calltree->v == 0)
			g_calltree->v = g_calltree->frames.alt_list;

		if(g_calltree->l == 0)	// base frame
		{
			g_calltree->v[g_calltree->l] = &g_calltree->frames.alt[0];
			g_calltree->v[g_calltree->l]->finalized = 0;
			g_calltree->v[g_calltree->l]->populated = 0;
			g_calltree->l++;
		}

		g_calltree->v[g_calltree->l] = &g_calltree->frames.alt[1];
		g_calltree->v[g_calltree->l]->finalized = 0;
		g_calltree->v[g_calltree->l]->populated = 0;
#if XAPI_RUNTIME_CHECKS
    g_calltree->v[g_calltree->l]->calling_frame = calling_frame;
#endif
		g_calltree->l++;

#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", g_calltree->x, g_calltree->l, calling_frame);
#endif
		
		// populate the alternate frame with an ENOMEM error
		frame_set(perrtab_SYS, SYS_ENOMEM, 0, 0, "libxapi::malloc", 1);
		xapi_frame_leave();

		return g_calltree->r = 1;
	}

	// push a frame for the base frame on the first push
	do
	{
		g_calltree->v[g_calltree->l] = g_calltree->frames.stor[g_calltree->l];
		g_calltree->v[g_calltree->l]->code = 0;
		g_calltree->v[g_calltree->l]->finalized = 0;
		g_calltree->v[g_calltree->l]->populated = 0;
		g_calltree->v[g_calltree->l]->filel = 0;
		g_calltree->v[g_calltree->l]->funcl = 0;
		g_calltree->l++;
	} while(g_calltree->l < 2);

#endif

  g_calltree->exe->l++;

#if XAPI_RUNTIME_CHECKS
	g_calltree->v[g_calltree->l - 1]->calling_frame = calling_frame;
#endif

#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", g_calltree->x, g_calltree->l, calling_frame);
#endif
	return g_calltree->r = 0;
}
#endif

#if 0
void API xapi_frame_leave3(const etable ** etab, int * code, int * rval)
{
#if DEVEL
	CT = g_calltree;
#endif
//printf("LEAVE :: [x=%2d][l=%2d] => ", g_calltree->x, g_calltree->l);

//	if((rc = g_calltree->v[g_calltree->x]->code))
//	if(((*code) = g_calltree->v[g_calltree->l - 1]->code))

	const etable * E = 0;
	int C = 0;
	int R = 0;

	if(g_calltree->v[g_calltree->l - 1]->code)
	{
		E = g_calltree->v[g_calltree->l - 1]->etab;
		C = g_calltree->v[g_calltree->l - 1]->code;
		R = (g_calltree->v[g_calltree->l - 1]->etab->id << 16) | g_calltree->v[g_calltree->l - 1]->code;

		// unwinding is underway
		if(--g_calltree->x == -1)
		{
//printf("[x=%2d][l=%2d] CALLSTACK_FREE\n", g_calltree->x, g_calltree->l);

			/*
			** x goes to -1 when a function exits that was not itself called with UNWIND-ing, e.g. main
			*/
			callstack_free();
		}
		else
		{
#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", g_calltree->x, g_calltree->l, g_calltree->v[g_calltree->l - 1]->calling_frame);
#endif
		}
	}

	// discard the frame
	else
	{
		if(--g_calltree->l == 0)
		{
//printf("[x=%2d][l=%2d] CALLSTACK_FREE\n", g_calltree->x, g_calltree->l);
			callstack_free();
		}
		else
		{
#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", g_calltree->x, g_calltree->l, g_calltree->v[g_calltree->l - 1]->calling_frame);
#endif
		}
	}

	if(etab) (*etab) = E;
	if(code) (*code) = C;
	if(rval) (*rval) = R;
}

int API xapi_frame_leave()
{
#if DEVEL
	CT = g_calltree;
#endif
	
	int rval;
	xapi_frame_leave3(0, 0, &rval);
	return rval;
}
#endif
