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
#include <unistd.h>

#include "internal.h"
#include "calltree.internal.h"
#include "errtab.internal.h"
#include "mm.internal.h"
#include "frame.internal.h"
#include "info.internal.h"
#include "exit.internal.h"

#if XAPI_RUNTIME_CHECKS
# include "frame.internal.h"
#endif

#include "macros.h"
#include "memblk.def.h"

__thread calltree * g_calltree;
__thread calltree * g_calltree_stor;
__thread int g_intent_to_fail;

//
// static
//

static void calltree_freeze(struct memblk * const restrict mb, calltree * restrict ct)
{
  int x;
  for(x = 0; x < ct->frames.l; x++)
    frame_freeze(mb, &ct->frames.v[x]);
  memblk_freeze(mb, &ct->frames.v);
}

/// calltree_unfreeze
//
//
//
static void calltree_unfreeze(struct memblk * const restrict mb, calltree * restrict ct)
{
  memblk_unfreeze(mb, &ct->frames.v);
  int x;
  for(x = 0; x < ct->frames.l; x++)
    frame_unfreeze(mb, &ct->frames.v[x]);
}

/// calltree_thaw
//
//
//
static void * __attribute__((nonnull)) calltree_thaw(calltree * restrict ct)
{
  memblk_thaw(ct, &ct->frames.v);
  int x;
  for(x = 0; x < ct->frames.l; x++)
    frame_thaw(ct, &ct->frames.v[x]);

  return ct;
}

//
// public
//

frame * calltree_frame_push()
{
  if(g_calltree_stor == 0)
    mm_malloc(&g_calltree_stor, sizeof(*g_calltree_stor));

  if(g_calltree == 0)
  {
    g_calltree = g_calltree_stor;
#if DEVEL
    S = g_calltree;
#endif
    memset(g_calltree, 0, sizeof(*g_calltree));
  }

  if(g_calltree->frames.l == INT_FAST32_MAX)
  {
    // stack overflow
  }

  mm_assure(&g_calltree->frames.v, &g_calltree->frames.l, &g_calltree->frames.a, sizeof(*g_calltree->frames.v), g_calltree->frames.l + 1);
  xapi_top_frame_index = g_calltree->frames.l;
  frame * f = &g_calltree->frames.v[g_calltree->frames.l++];
  memset(f, 0, sizeof(*f));
  return f;
}

void calltree_teardown()
{
  g_calltree = 0;
  g_calltree_stor = 0;
}

//
// api
//

API void xapi_calltree_unwind()
{
  g_calltree = 0;
#if DEVEL
  S = g_calltree;
#endif

  xapi_top_frame_index = -1;
  xapi_infos_stagingl = 0;

#if XAPI_RUNTIME_CHECKS
  xapi_stack_raised_exit = 0;
#endif
}

API memblk * xapi_calltree_freeze()
{
  memblk * const mb = &mm_mb;

  // freeze the calltree on this thread
  calltree_freeze(mb, g_calltree);

  return mb;
}

API void xapi_calltree_unfreeze()
{
  memblk * const mb = &mm_mb;

  calltree_unfreeze(mb, g_calltree);
}

API calltree * xapi_calltree_thaw(void * mb)
{
  return calltree_thaw(g_calltree = mb);
}

API calltree * xapi_calltree_export(void * dst, size_t sz)
{
  if(memblk_size(&mm_mb) > sz)
    return 0;

  memblk * mb = xapi_calltree_freeze();
  memblk_copyto(mb, dst, sz);
  xapi_calltree_unfreeze();

  return calltree_thaw(dst);
}

API xapi_code xapi_calltree_errcode()
{
  return xapi_exit_errcode(g_calltree->exit);
}

API xapi xapi_calltree_errval()
{
  return g_calltree->exit;
}

API const errtab * xapi_calltree_errtab()
{
  return xapi_exit_errtab(g_calltree->exit);
}
