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
#include <inttypes.h>

#include "xapi.h"
#include "frame.internal.h"
#include "calltree.internal.h"
#include "mm.internal.h"
#include "info.internal.h"
#include "errtab/XAPI.errtab.h"
#include "exit.internal.h"

#include "memblk.def.h"
#include "macros.h"

#if XAPI_RUNTIME_CHECKS
#include <stdlib.h>
#include <stdio.h>

__thread         struct frame_addresses g_frame_addresses;
__thread void * APIDATA xapi_calling_frame_address;
__thread void * APIDATA xapi_caller_frame_address;
__thread xapi APIDATA xapi_stack_raised_exit;
#endif

// per-thread sentinels
__thread int APIDATA xapi_sentinel;
__thread xapi_frame_index APIDATA xapi_top_frame_index = -1;

//
// static
//

static void frame_set(
    xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
{
#if XAPI_RUNTIME_CHECKS
  // code and table are specified together
  if(!(exit >> 16) ^ !(exit & 0xFFFF))
  {
    if(!(exit & 0xFFFF))
      exit = XAPI_NOCODE;
    else
      exit = XAPI_NOTABLE;
  }
#endif

  // allocate a frame in the calltree
  frame * f = calltree_frame_push();

#if XAPI_RUNTIME_CHECKS
  // code and table are required for the base frame
  if(g_calltree->frames.l == 1 && (!(exit >> 16) || !(exit & 0xFFFF)))
  {
    if(!(exit & 0xFFFF))
      exit = XAPI_NOCODE;
    else
      exit = XAPI_NOTABLE;
  }
#endif

  // save frame info
  f->parent_index = parent_index;
  f->line = line;
  f->infos.l = 0;

  size_t funcl = strlen(func);
  size_t filel = 0;
  if(file)
    filel = strlen(file);

  mm_sloadw(&f->file, &f->filel, file, filel);
  mm_sloadw(&f->func, &f->funcl, func, funcl);

  if(exit)
  {
    f->exit = exit;
    if(g_calltree->frames.l == 1)
      g_calltree->exit = exit;

    // apply any staged infos
    mm_assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + xapi_infos_stagingl);
    int x;
    for(x = 0; x < xapi_infos_stagingl; x++)
      f->infos.v[x] = xapi_infos_staging[x];
    f->infos.l += xapi_infos_stagingl;
    xapi_info_unstage();
  }

#if XAPI_RUNTIME_CHECKS
  if(exit == XAPI_NOFATAL)
    xapi_stack_raised_exit = exit;
#endif
}

//
// public
//

void frame_teardown()
{
#if XAPI_RUNTIME_CHECKS
  free(g_frame_addresses.v);
  g_frame_addresses.v = 0;
#endif
}

void frame_freeze(memblk * const restrict mb, frame * restrict f)
{
  memblk_freeze(mb, &f->file);
  memblk_freeze(mb, &f->func);

  int x;
  for(x = 0; x < f->infos.l; x++)
    info_freeze(mb, &f->infos.v[x]);
  memblk_freeze(mb, &f->infos.v);
}

void frame_unfreeze(memblk * const restrict mb, frame * restrict f)
{
  memblk_unfreeze(mb, &f->file);
  memblk_unfreeze(mb, &f->func);

  memblk_unfreeze(mb, &f->infos.v);
  int x;
  for(x = 0; x < f->infos.l; x++)
    info_unfreeze(mb, &f->infos.v[x]);
}

void frame_thaw(void * restrict mb, frame * restrict f)
{
  memblk_thaw(mb, &f->file);
  memblk_thaw(mb, &f->func);

  memblk_thaw(mb, &f->infos.v);
  int x;
  for(x = 0; x < f->infos.l; x++)
    info_thaw(mb, &f->infos.v[x]);
}

//
// api
//

#if XAPI_RUNTIME_CHECKS
void API xapi_record_frame(void * calling_frame)
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
    *(void**)dst = realloc(*(void**)dst, ns * z);
    (*dsta) = ns;
  }

  g_frame_addresses.v[g_frame_addresses.l++] = calling_frame;
}
#endif

xapi API xapi_frame_leave(int topframe)
{
#if XAPI_RUNTIME_CHECKS
  if(g_frame_addresses.l)   // pop the frame
    xapi_caller_frame_address = g_frame_addresses.v[--g_frame_addresses.l];
#endif

  xapi exit = 0;
  if(g_calltree)
    exit = g_calltree->exit;

  if(topframe)
  {
    xapi_calltree_unwind();
    xapi_sentinel = 0;
#if XAPI_RUNTIME_CHECKS
    free(g_frame_addresses.v);
    g_frame_addresses.v = 0;
    g_frame_addresses.l = 0;
    g_frame_addresses.a = 0;
    xapi_calling_frame_address = 0;
#endif

    mm_teardown();
    calltree_teardown();
    info_teardown();
  }

  return exit;
}

int API xapi_unwinding()
{
  return !!g_calltree;
}

xapi API xapi_frame_errval(xapi_frame_index index)
{
  return g_calltree->frames.v[index].exit;
}

void API xapi_frame_set(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const file
  , const int line
  , const char * const func
)
{
  frame_set(exit, parent_index, file, line, func);
}

void API xapi_frame_set_infos(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vstr
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
{
  frame_set(exit, parent_index, file, line, func);
  xapi_frame_info_pushs(key, vstr);
}

void API xapi_frame_set_infow(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vbuf
  , const size_t vlen
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
{
  frame_set(exit, parent_index, file, line, func);
  xapi_frame_info_pushw(key, vbuf, vlen);
}

void API xapi_frame_set_infof(
    const xapi exit
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vfmt
  , const char * const restrict file
  , const int line
  , const char * const restrict func
  , ...
)
{
  frame_set(exit, parent_index, file, line, func);

  va_list va;
  va_start(va, func);
  xapi_frame_info_pushvf(key, vfmt, va);
  va_end(va);
}

void API xapi_frame_info_pushw(const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  if(g_calltree && key)
  {
    size_t keyl = strlen(key);
    if(keyl)
    {
      frame * f = &g_calltree->frames.v[g_calltree->frames.l - 1];

      // ensure allocation for the info list
      mm_assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + 1);
      info * i = &f->infos.v[f->infos.l++];

      info_setw(i, key, keyl, vbuf, vlen);
    }
  }
}

void API xapi_frame_info_pushs(const char * restrict key, const char * restrict vstr)
{
  xapi_frame_info_pushw(key, vstr, strlen(vstr));
}

void API xapi_frame_info_pushvf(const char * restrict key, const char * restrict vfmt, va_list va)
{
  if(g_calltree && key)
  {
    size_t keyl = strlen(key);
    if(keyl)
    {
      frame * f = &g_calltree->frames.v[g_calltree->frames.l - 1];

      // ensure allocation for the info list
      mm_assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + 1);
      info * i = &f->infos.v[f->infos.l++];

      info_setvf(i, key, keyl, vfmt, va);
    }
  }
}

void API xapi_frame_info_pushf(const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xapi_frame_info_pushvf(key, vfmt, va);

  va_end(va);
}
