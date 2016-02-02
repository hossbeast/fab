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
#include <inttypes.h>

#include "internal.h"
#include "frame.internal.h"
#include "calltree.internal.h"
#include "mm.internal.h"
#include "stack.h"
#include "info.h"
#include "errtab/XAPI.errtab.h"

#include "memblk.def.h"
#include "macros.h"

#define restrict __restrict

#if XAPI_RUNTIME_CHECKS
#include <stdlib.h>
#include <stdio.h>

__thread         struct frame_addresses g_frame_addresses;
__thread APIDATA void * xapi_calling_frame_address;
__thread APIDATA void * xapi_caller_frame_address;
__thread APIDATA const etable * xapi_stack_raised_etab;
__thread APIDATA int xapi_stack_raised_code;
#endif

// per-thread sentinels
__thread APIDATA int xapi_sentinel;

static stack * frame_set(stack * s, const etable * restrict etab, xapi_code code, const char * const restrict file, const int line, const char * const restrict func)
{
if(etab)
  printf("frame_set %s:%d\n", etab->name, code);
else
  printf("frame_set %p:%d\n", etab, code);

#if XAPI_RUNTIME_CHECKS
  // code/table are required for the base frame
  if((code == 0) ^ (etab == 0))
  {
    etab = perrtab_XAPI;
    code = XAPI_ILLFAIL;
  }
#endif

  // allocate a new stack when throwing a new error
  if(code)
    stack_push();

#if XAPI_RUNTIME_CHECKS
  else if(!g_stack)
  {
    etab == perrtab_XAPI;
    code = XAPI_ILLFAIL;
  }
#endif

  // save site-of-error information on the stack
  if(g_stack->frames.l == 0)
  {
#if XAPI_RUNTIME_CHECKS
    // code/table are required for the base frame
    if(!code)
    {
      etab = perrtab_XAPI;
      code = XAPI_ILLFAIL;
    }
#endif

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

#if XAPI_RUNTIME_CHECKS
  if(etab == perrtab_XAPI && code == XAPI_NOFATAL)
  {
    xapi_stack_raised_etab = etab;
    xapi_stack_raised_code = code;
  }
#endif

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

API xapi xapi_frame_leave(int topframe)
{
  xapi r = 0;
  xapi_frame_leave3(topframe, 0, 0, &r);
  return r;
}

API void xapi_frame_leave3(int topframe, const etable ** restrict etab, xapi_code * const restrict code, xapi * const restrict rval)
{
#if XAPI_RUNTIME_CHECKS
  if(g_frame_addresses.l)   // pop the frame
    xapi_caller_frame_address = g_frame_addresses.v[--g_frame_addresses.l];
#endif

  const etable * E = 0;
  xapi_code C = 0;
  xapi R = 0;

  if(g_stack)
  {
    E = g_stack->etab;
    C = g_stack->code;
  }

#if XAPI_RUNTIME_CHECKS
  if(xapi_stack_raised_etab)
  {
    E = xapi_stack_raised_etab;
    C = xapi_stack_raised_code;
  }
#endif

  if(E || C)
    R = (E->id << 16) | C;

  if(topframe)
  {
    g_stack = 0;
#if DEVEL
  S = g_stack;
#endif

    xapi_sentinel = 0;
#if XAPI_RUNTIME_CHECKS
    g_frame_addresses.l = 0;
    xapi_calling_frame_address = 0;
    xapi_caller_frame_address = 0;
    xapi_stack_raised_etab = 0;
    xapi_stack_raised_code = 0;
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

API xapi_code xapi_frame_errcode()
{
	return g_stack->code;
}

API xapi xapi_frame_errval()
{
  return (g_stack->etab->id << 16) | g_stack->code;
}

API const etable * xapi_frame_errtab()
{
	return g_stack->etab;
}

API stack * xapi_frame_set(stack * s, const etable * const etab, const xapi_code code, const char * const file, const int line, const char * const func)
{
	return frame_set(s, etab, code, file, line, func);
}

API stack * xapi_frame_set_messagew(stack * s, const etable * const etab, const xapi_code code, const char * const msg, int msgl, const char * const file, const int line, const char * const func)
{
	s = frame_set(s, etab, code, file, line, func);
	msgl = msgl ?: msg ? strlen(msg) : 0;

  sloadw(&s->msg, &s->msgl, &s->msga, msg, msgl);

  return s;
}

API stack * xapi_frame_set_messagef(stack * s, const etable * const etab, const xapi_code code, const char * const fmt, const char * const file, const int line, const char * const func, ...)
{
	s = frame_set(s, etab, code, file, line, func);

  // save the msg when setting the base frame
  va_list va;
  va_start(va, func);
  svloadf(&s->msg, &s->msgl, &s->msga, fmt, va);
  va_end(va);

  return s;
}

API void xapi_frame_infow(const char * const k, int kl, const char * const v, int vl)
{
  if(g_stack)
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
}

API void xapi_frame_infof(const char * const k, int kl, const char * const vfmt, ...)
{
  if(g_stack)
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
}
