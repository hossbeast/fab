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
#include "info.internal.h"
#include "errtab/XAPI.errtab.h"
#include "error.internal.h"

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
__thread APIDATA xapi_frame_index xapi_top_frame_index = -1;
__thread int g_fail_intent;

//
// static
//

static void frame_set(
    const etable * restrict etab
  , xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
{
#if XAPI_RUNTIME_CHECKS
  // code and table are specified together
  if(!code ^ !etab)
  {
    if(!code)
      code = XAPI_NOCODE;
    else if(!etab)
      code = XAPI_NOTABLE;
    etab = perrtab_XAPI;
  }
#endif

  // allocate a frame in the calltree
  frame * f = calltree_frame_push();

#if XAPI_RUNTIME_CHECKS
  // code and table are required for the base frame
  if(g_calltree->frames.l == 1 && (!code || !etab))
  {
    if(!code)
      code = XAPI_NOCODE;
    else if(!etab)
      code = XAPI_NOTABLE;
    etab = perrtab_XAPI;
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

  if(code)
  {
    mm_malloc(&f->error, sizeof(*f->error));
    f->error->etab = etab;
    f->error->code = code;

    if(g_calltree->frames.l == 1)
    {
      g_calltree->exit_code = code;
      g_calltree->exit_table = etab;
      g_calltree->exit_value = error_errval(g_calltree->frames.v[0].error);
    }

    // apply any staged infos
    mm_assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + info_stagingl);
    int x;
    for(x = 0; x < info_stagingl; x++)
      f->infos.v[x] = info_staging[x];
    f->infos.l += info_stagingl;
  }

#if XAPI_RUNTIME_CHECKS
  if(etab == perrtab_XAPI && code == XAPI_NOFATAL)
  {
    xapi_stack_raised_etab = etab;
    xapi_stack_raised_code = code;
  }
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
  if(f->error)
    error_freeze(mb, f->error);
  memblk_freeze(mb, &f->error);
  memblk_freeze(mb, &f->file);
  memblk_freeze(mb, &f->func);

  int x;
  for(x = 0; x < f->infos.l; x++)
    info_freeze(mb, &f->infos.v[x]);
  memblk_freeze(mb, &f->infos.v);
}

void frame_unfreeze(memblk * const restrict mb, frame * restrict f)
{
  memblk_unfreeze(mb, &f->error);
  if(f->error)
    error_unfreeze(mb, f->error);
	memblk_unfreeze(mb, &f->file);
	memblk_unfreeze(mb, &f->func);

	memblk_unfreeze(mb, &f->infos.v);
	int x;
	for(x = 0; x < f->infos.l; x++)
		info_unfreeze(mb, &f->infos.v[x]);
}

void frame_thaw(char * const restrict mb, frame * restrict f)
{
  memblk_thaw(mb, &f->error);
  if(f->error)
    error_thaw(mb, f->error);
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

API void xapi_fail_intent()
{
  g_fail_intent = 1;
}

API xapi xapi_frame_leave(int topframe)
{
#if XAPI_RUNTIME_CHECKS
  if(g_frame_addresses.l)   // pop the frame
    xapi_caller_frame_address = g_frame_addresses.v[--g_frame_addresses.l];
#endif

  xapi exit = 0;
  if(g_calltree)
    exit = g_calltree->exit_value;

  if(topframe)
  {
    xapi_calltree_unwind();
    xapi_sentinel = 0;
#if XAPI_RUNTIME_CHECKS
    xapi_calling_frame_address = 0;
#endif
  }

  return exit;
}

API int xapi_unwinding()
{
  return !!g_calltree;
}

API xapi xapi_frame_errval(xapi_frame_index index)
{
  return error_errval(g_calltree->frames.v[index].error);
}

API void xapi_frame_set(
    const etable * const etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const file
  , const int line
  , const char * const func
)
{
	frame_set(etab, code, parent_index, file, line, func);
}

API void xapi_frame_set_infos(
    const etable * const restrict etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vstr
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
{
	frame_set(etab, code, parent_index, file, line, func);
  xapi_info_adds(key, vstr);
}

API void xapi_frame_set_infow(
    const etable * const restrict etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vbuf
  , const size_t vlen
  , const char * const restrict file
  , const int line
  , const char * const restrict func
)
{
	frame_set(etab, code, parent_index, file, line, func);
  xapi_info_addw(key, vbuf, vlen);
}

API void xapi_frame_set_infof(
    const etable * const restrict etab
  , const xapi_code code
  , const xapi_frame_index parent_index
  , const char * const restrict key
  , const char * const restrict vfmt
  , const char * const restrict file
  , const int line
  , const char * const restrict func
  , ...
)
{
	frame_set(etab, code, parent_index, file, line, func);

  va_list va;
  va_start(va, func);
  xapi_info_vaddf(key, vfmt, va);
  va_end(va);
}
