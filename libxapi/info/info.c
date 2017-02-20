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

#include <string.h>

#include "internal.h"
#include "info.internal.h"
#include "calltree.internal.h"
#include "frame.internal.h"
#include "mm.internal.h"

#include "macros.h"
#include "memblk.def.h"

#define restrict __restrict

// per-thread info staging
__thread APIDATA info * info_staging;
__thread APIDATA size_t info_stagingl;
__thread APIDATA size_t info_staginga;

//
// public
//

void info_teardown()
{
  info_staging = 0;
  info_stagingl = 0;
  info_staginga = 0;
}

void info_freeze(memblk * const restrict mb, info * restrict i)
{
  memblk_freeze(mb, &i->ks);
  memblk_freeze(mb, &i->vs);
}

void info_unfreeze(memblk * const restrict mb, info * restrict i)
{
  memblk_unfreeze(mb, &i->ks);
  memblk_unfreeze(mb, &i->vs);
}

void info_thaw(char * const restrict mb, info * restrict i)
{
  memblk_thaw(mb, &i->ks);
  memblk_thaw(mb, &i->vs);
}

//
// api
//

API void xapi_info_adds(const char * const key, const char * const vstr)
{
  xapi_info_addw(key, vstr, strlen(vstr));
}

API void xapi_info_addw(const char * const key, const char * const vbuf, size_t vlen)
{
  if(g_calltree || g_fail_intent)
  {
    if(key && vbuf && vlen)
    {
      info * i = 0;
      if(g_calltree)
      {
        frame * f = &g_calltree->frames.v[g_calltree->frames.l - 1];

        // ensure allocation for the info list
        mm_assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + 1);
        i = &f->infos.v[f->infos.l++];
      }
      else
      {
        // if not unwinding, stage these infos for inclusion into the base frame
        mm_assure(&info_staging, &info_stagingl, &info_staginga, sizeof(*info_staging), info_stagingl + 1);
        i = &info_staging[info_stagingl++];
      }

      memset(i, 0, sizeof(*i));
      mm_sloadw(&i->ks, &i->kl, key, strlen(key));
      mm_sloadw(&i->vs, &i->vl, vbuf, vlen);
    }
  }
}

API void xapi_info_addf(const char * const key, const char * const vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xapi_info_vaddf(key, vfmt, va);

  va_end(va);
}

API void xapi_info_vaddf(const char * const key, const char * const vfmt, va_list va)
{
  if(g_calltree || g_fail_intent)
  {
    // measure
    va_list va2;
    va_copy(va2, va);
    size_t vlen = vsnprintf(0, 0, vfmt, va2);
    va_end(va2);

    if(key && vfmt && vlen)
    {
      info * i = 0;
      if(g_calltree)
      {
        frame * f = &g_calltree->frames.v[g_calltree->frames.l - 1];

        // ensure allocation for the info list
        mm_assure(&f->infos.v, &f->infos.l, &f->infos.a, sizeof(*f->infos.v), f->infos.l + 1);
        i = &f->infos.v[f->infos.l++];
      }
      else
      {
        // if not unwinding, stage these infos for inclusion into the base frame
        mm_assure(&info_staging, &info_stagingl, &info_staginga, sizeof(*info_staging), info_stagingl + 1);
        i = &info_staging[info_stagingl++];
      }

      memset(i, 0, sizeof(*i));
      mm_sloadw(&i->ks, &i->kl, key, strlen(key));
      mm_svloadf(&i->vs, &i->vl, vfmt, va);
    }
  }
}
