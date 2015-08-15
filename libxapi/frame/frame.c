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
#include <stdlib.h>
#include <stdio.h>

__thread struct frame_addresses g_frame_addresses;
__thread void * APIDATA xapi_calling_frame_address;
__thread void * APIDATA xapi_caller_frame_address;
#endif

// per-thread sentinels
__thread int APIDATA xapi_sentinel;

static stack * frame_set(stack * s, const struct etable * const restrict etab, const int16_t code, const char * const restrict file, const int line, const char * const restrict func)
{
  // allocate a new stack when throwing a new error
  if(code)
    stack_push();

  // save site-of-error information on the stack
  if(g_stack->frames.l == 0)
  {
    if(code == 0)
    {
      // fail(0)
#if XAPI_RUNTIME_CHECKS
  dprintf(2, "FATAL : error raised without code\n");
#endif
    }

    g_stack->etab = etab;
    g_stack->code = code;
    g_stack->msgl = 0;
  }

  // allocate a new frame
  assure(&g_stack->frames.v, &g_stack->frames.l, &g_stack->frames.a, sizeof(*g_stack->frames.v), g_stack->frames.l + 1);
	frame * f = &g_stack->frames.v[g_stack->frames.l++];
  memset(f, 0, sizeof(*f));

  // save frame info
  f->line = line;
  f->infos.l = 0;

  size_t funcl = strlen(func);
  size_t filel = 0;
  if(file)
    filel = strlen(file);

  sloadw(&f->file, &f->filel, &f->filea, file, filel);
  sloadw(&f->func, &f->funcl, &f->funca, func, funcl);

  // restore the stack for this frame
  if(s)
  {
    g_stack = s;

#if DEVEL
	S = g_stack;
#endif
  }

  // return the stack that this frame belongs to
  return g_stack;
}

//
// public
//

void frame_teardown()
{
#if XAPI_RUNTIME_CHECKS
  free(g_frame_addresses.v);
#endif
}

void frame_freeze(memblk * const restrict mb, frame * f)
{
	memblk_freeze(mb, &f->file);
	memblk_freeze(mb, &f->func);

	int x;
	for(x = 0; x < f->infos.l; x++)
		info_freeze(mb, &f->infos.v[x]);
	memblk_freeze(mb, &f->infos.v);

  if(f->child)
  	stack_freeze(mb, f->child);
	memblk_freeze(mb, &f->child);
}

void frame_unfreeze(memblk * const restrict mb, frame * f)
{
	memblk_unfreeze(mb, &f->file);
	memblk_unfreeze(mb, &f->func);

	memblk_unfreeze(mb, &f->infos.v);
	int x;
	for(x = 0; x < f->infos.l; x++)
		info_unfreeze(mb, &f->infos.v[x]);

  memblk_unfreeze(mb, &f->child);
  if(f->child)
    stack_unfreeze(mb, f->child);
}

void frame_thaw(char * const restrict mb, frame * f)
{
	memblk_thaw(mb, &f->file);
	memblk_thaw(mb, &f->func);

	memblk_thaw(mb, &f->infos.v);
	int x;
	for(x = 0; x < f->infos.l; x++)
		info_thaw(mb, &f->infos.v[x]);

	memblk_thaw(mb, &f->child);
  if(f->child)
    stack_thaw(mb, f->child);
}

///
/// calltree manipulation
///

#if XAPI_RUNTIME_CHECKS
API void xapi_record_frame(void * calling_frame)
{
  void * dst = &g_frame_addresses.v;
  size_t * dsta = &g_frame_addresses.a;
  size_t z = sizeof(*g_frame_addresses.v);
  size_t l = g_frame_addresses.l + 1;

	if(l > (*dsta))
	{
		size_t ns = (*dsta) ?: 10;
		while(ns <= l)
			ns = ns * 2 + ns / 2;
    if((*(void**)dst = realloc(*(void**)dst, ns * z)) == 0)
    {
      /* failure to allocate */
    }
		(*dsta) = ns;
	}

  g_frame_addresses.v[g_frame_addresses.l++] = calling_frame;
}
#endif

API int xapi_frame_leave(int sentinel)
{
  int r = 0;
  xapi_frame_leave3(sentinel, 0, 0, &r);
  return r;
}

API void xapi_frame_leave3(int sentinel, const etable ** restrict etab, int * const restrict code, int * const restrict rval)
{
#if XAPI_RUNTIME_CHECKS
  if(g_frame_addresses.l)   // pop the frame
    xapi_caller_frame_address = g_frame_addresses.v[--g_frame_addresses.l];
#endif

  const etable * E = 0;
  int C = 0;
  int R = 0;

  if(g_stack)
  {
    E = g_stack->etab;
    C = g_stack->code;
    if(E)
      R = (E->id << 16) | C;
  }

  if(sentinel)
  {
    g_stack = 0;
    xapi_sentinel = 0;
#if XAPI_RUNTIME_CHECKS
    g_frame_addresses.l = 0;
    xapi_calling_frame_address = 0;
    xapi_caller_frame_address = 0;
#endif
  }

	if(etab) (*etab) = E;
	if(code) (*code) = C;
	if(rval) (*rval) = R;
}

API int xapi_unwinding()
{
  return !!g_stack;
}

API int xapi_frame_errcode()
{
	return g_stack->code;
}

API int xapi_frame_errval()
{
  return (g_stack->etab->id << 16) | g_stack->code;
}

API const etable * xapi_frame_errtab()
{
	return g_stack->etab;
}

API stack * xapi_frame_set(stack * s, const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
	return frame_set(s, etab, code, file, line, func);
}

API stack * xapi_frame_set_messagew(stack * s, const etable * const etab, const int16_t code, const char * const msg, int msgl, const char * const file, const int line, const char * const func)
{
	s = frame_set(s, etab, code, file, line, func);
	msgl = msgl ?: msg ? strlen(msg) : 0;

  // save the msg when setting the base frame for the stack
	if(s->frames.l == 1)
    sloadw(&s->msg, &s->msgl, &s->msga, msg, msgl);

  return s;
}

API stack * xapi_frame_set_messagef(stack * s, const etable * const etab, const int16_t code, const char * const fmt, const char * const file, const int line, const char * const func, ...)
{
	s = frame_set(s, etab, code, file, line, func);

  // save the msg when setting the base frame for the stack
	if(s->frames.l == 1)
  {
    va_list va;
    va_start(va, func);
    svloadf(&s->msg, &s->msgl, &s->msga, fmt, va);
    va_end(va);
  }

  return s;
}

API void xapi_frame_infow(const char * const k, int kl, const char * const v, int vl)
{
  if(k)
    kl = kl ?: strlen(k);
  if(v)
    vl = vl ?: strlen(v);

  if(k && kl && v && vl)
  {
    frame * f = &g_stack->frames.v[g_stack->frames.l - 1];

    // ensure allocation for the info list
    assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + 1);
    info * i = &f->infos.v[f->infos.l++];
    memset(i, 0, sizeof(*i));

    // save the strings
    sloadw(&i->ks, &i->kl, &i->ka, k, kl);
    sloadw(&i->vs, &i->vl, &i->va, v, vl);
  }
}

API void xapi_frame_infof(const char * const k, int kl, const char * const vfmt, ...)
{
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
    frame * f = &g_stack->frames.v[g_stack->frames.l - 1];

    // ensure allocation for the info list
    assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + 1);
    info * i = &f->infos.v[f->infos.l++];
    memset(i, 0, sizeof(*i));

    // save the strings
    sloadw(&i->ks, &i->kl, &i->ka, k, kl);
    svloadf(&i->vs, &i->vl, &i->va, vfmt, va);
  }

  va_end(va);
}
