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

// per-thread info staging
__thread info * APIDATA xapi_infos_staging;
__thread size_t APIDATA xapi_infos_stagingl;
__thread size_t APIDATA xapi_infos_staginga;

//
// public
//

void info_teardown()
{
  xapi_infos_staging = 0;
  xapi_infos_stagingl = 0;
  xapi_infos_staginga = 0;
}

void info_setw(info * restrict i, const char * restrict key, size_t keyl, const char * restrict vbuf, size_t vlen)
{
  memset(i, 0, sizeof(*i));
  mm_sloadw(&i->ks, &i->kl, key, keyl);
  if(vbuf && vlen)
    mm_sloadw(&i->vs, &i->vl, vbuf, vlen);
  else
    mm_sloadw(&i->vs, &i->vl, "", 0);
}

void info_setvf(info * restrict i, const char * restrict key, size_t keyl, const char * restrict vfmt, va_list va)
{
  if(vfmt == 0)
    return;

  va_list va2;
  va_copy(va2, va);
  size_t vlen = vsnprintf(0, 0, vfmt, va2);
  va_end(va2);

  memset(i, 0, sizeof(*i));
  mm_sloadw(&i->ks, &i->kl, key, keyl);
  if(vfmt && vlen)
  {
    mm_svloadf(&i->vs, &i->vl, vfmt, va);
  }
  else
  {
    vsnprintf(0, 0, vfmt, va);  // discard
    mm_sloadw(&i->vs, &i->vl, "", 0);
  }
}

void info_freeze(memblk * restrict mb, info * restrict i)
{
  memblk_freeze(mb, &i->ks);
  memblk_freeze(mb, &i->vs);
}

void info_unfreeze(memblk * restrict mb, info * restrict i)
{
  memblk_unfreeze(mb, &i->ks);
  memblk_unfreeze(mb, &i->vs);
}

void info_thaw(char * restrict mb, info * restrict i)
{
  memblk_thaw(mb, &i->ks);
  memblk_thaw(mb, &i->vs);
}

//
// api
//

void API xapi_info_unstage()
{
  xapi_infos_stagingl = 0;
}

void API xapi_info_inserts(uint16_t index, const char * restrict key, const char * restrict vstr)
{
  xapi_info_insertw(index, key, vstr, strlen(vstr));
}

void API xapi_info_insertw(uint16_t index, const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  if(key)
  {
    size_t keyl = strlen(key);
    if(keyl)
    {
      mm_assure(&xapi_infos_staging, &xapi_infos_stagingl, &xapi_infos_staginga, sizeof(*xapi_infos_staging), xapi_infos_stagingl + 1);

      // displace
      memmove(
          xapi_infos_staging + index + 1
        , xapi_infos_staging + index
        , (xapi_infos_stagingl - index) * sizeof(*xapi_infos_staging)
      );

      info * i = &xapi_infos_staging[index];
      xapi_infos_stagingl++;

      info_setw(i, key, keyl, vbuf, vlen);
    }
  }
}

void API xapi_info_insertvf(uint16_t index, const char * restrict key, const char * restrict vfmt, va_list va)
{
  if(key)
  {
    size_t keyl = strlen(key);
    if(keyl)
    {
      // if not unwinding, stage these infos for inclusion into the base frame
      mm_assure(&xapi_infos_staging, &xapi_infos_stagingl, &xapi_infos_staginga, sizeof(*xapi_infos_staging), xapi_infos_stagingl + 1);

      // displace
      memmove(
          xapi_infos_staging + index + 1
        , xapi_infos_staging + index
        , (xapi_infos_stagingl - index) * sizeof(*xapi_infos_staging)
      );

      info * i = &xapi_infos_staging[index];
      xapi_infos_stagingl++;

      info_setvf(i, key, keyl, vfmt, va);
    }
  }
}

void API xapi_info_insertf(uint16_t index, const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xapi_info_insertvf(index, key, vfmt, va);

  va_end(va);
}


void API xapi_info_pushs(const char * restrict key, const char * restrict vstr)
{
  xapi_info_pushw(key, vstr, strlen(vstr));
}

void API xapi_info_pushw(const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  xapi_info_insertw(xapi_infos_stagingl, key, vbuf, vlen);
}

void API xapi_info_pushvf(const char * restrict key, const char * restrict vfmt, va_list va)
{
  xapi_info_insertvf(xapi_infos_stagingl, key, vfmt, va);
}

void API xapi_info_pushf(const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xapi_info_insertvf(xapi_infos_stagingl, key, vfmt, va);

  va_end(va);
}

void API xapi_info_unshifts(const char * restrict key, const char * restrict vstr)
{
  xapi_info_insertw(xapi_infos_stagingl, key, vstr, strlen(vstr));
}

void API xapi_info_unshiftw(const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  xapi_info_insertw(xapi_infos_stagingl, key, vbuf, vlen);
}

void API xapi_info_unshiftvf(const char * restrict key, const char * restrict vfmt, va_list va)
{
  if(key)
  {
    size_t keyl = strlen(key);
    if(keyl)
    {
      // if not unwinding, stage these infos for inclusion into the base frame
      mm_assure(&xapi_infos_staging, &xapi_infos_stagingl, &xapi_infos_staginga, sizeof(*xapi_infos_staging), xapi_infos_stagingl + 1);

      // displace
      memmove(
          xapi_infos_staging + 1
        , xapi_infos_staging
        , xapi_infos_stagingl * sizeof(*xapi_infos_staging)
      );

      info * i = &xapi_infos_staging[0];
      xapi_infos_stagingl++;

      info_setvf(i, key, keyl, vfmt, va);
    }
  }
}

void API xapi_info_unshiftf(const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xapi_info_unshiftvf(key, vfmt, va);

  va_end(va);
}
