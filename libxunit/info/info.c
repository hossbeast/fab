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
#include <stdio.h>

#include "info.internal.h"
#include "mm.internal.h"

#include "macros.h"

// per-thread info staging
__thread info * APIDATA xunit_infos_staging;
__thread size_t APIDATA xunit_infos_stagingl;
__thread size_t APIDATA xunit_infos_staginga;

//
// public
//

void info_teardown()
{
  xunit_infos_staging = 0;
  xunit_infos_stagingl = 0;
  xunit_infos_staginga = 0;
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
  va_list va2;
  size_t vlen;

  if(vfmt == 0)
    return;

  va_copy(va2, va);
  vlen = vsnprintf(0, 0, vfmt, va2);
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

//
// api
//

void API xunit_info_unstage()
{
  xunit_infos_stagingl = 0;
}

void API xunit_info_inserts(uint16_t index, const char * restrict key, const char * restrict vstr)
{
  xunit_info_insertw(index, key, vstr, strlen(vstr));
}

void API xunit_info_insertw(uint16_t index, const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  size_t keyl;

  if(!key) {
    return;
  }

  if((keyl = strlen(key)))
  {
    mm_assure(&xunit_infos_staging, &xunit_infos_stagingl, &xunit_infos_staginga, sizeof(*xunit_infos_staging), xunit_infos_stagingl + 1);

    // displace
    memmove(
        xunit_infos_staging + index + 1
      , xunit_infos_staging + index
      , (xunit_infos_stagingl - index) * sizeof(*xunit_infos_staging)
    );

    info * i = &xunit_infos_staging[index];
    xunit_infos_stagingl++;

    info_setw(i, key, keyl, vbuf, vlen);
  }
}

void API xunit_info_insertvf(uint16_t index, const char * restrict key, const char * restrict vfmt, va_list va)
{
  size_t keyl;

  if(!key) {
    return;
  }

  if((keyl = strlen(key)))
  {
    // if not unwinding, stage these infos for inclusion into the base frame
    mm_assure(&xunit_infos_staging, &xunit_infos_stagingl, &xunit_infos_staginga, sizeof(*xunit_infos_staging), xunit_infos_stagingl + 1);

    // displace
    memmove(
        xunit_infos_staging + index + 1
      , xunit_infos_staging + index
      , (xunit_infos_stagingl - index) * sizeof(*xunit_infos_staging)
    );

    info * i = &xunit_infos_staging[index];
    xunit_infos_stagingl++;

    info_setvf(i, key, keyl, vfmt, va);
  }
}

void API xunit_info_insertf(uint16_t index, const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xunit_info_insertvf(index, key, vfmt, va);

  va_end(va);
}


void API xunit_info_pushs(const char * restrict key, const char * restrict vstr)
{
  xunit_info_pushw(key, vstr, strlen(vstr));
}

void API xunit_info_pushw(const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  xunit_info_insertw(xunit_infos_stagingl, key, vbuf, vlen);
}

void API xunit_info_pushvf(const char * restrict key, const char * restrict vfmt, va_list va)
{
  xunit_info_insertvf(xunit_infos_stagingl, key, vfmt, va);
}

void API xunit_info_pushf(const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xunit_info_insertvf(xunit_infos_stagingl, key, vfmt, va);

  va_end(va);
}

void API xunit_info_unshifts(const char * restrict key, const char * restrict vstr)
{
  xunit_info_insertw(xunit_infos_stagingl, key, vstr, strlen(vstr));
}

void API xunit_info_unshiftw(const char * restrict key, const char * restrict vbuf, size_t vlen)
{
  xunit_info_insertw(xunit_infos_stagingl, key, vbuf, vlen);
}

void API xunit_info_unshiftvf(const char * restrict key, const char * restrict vfmt, va_list va)
{
  size_t keyl;

  if(!key) {
    return;
  }
  if((keyl = strlen(key)))
  {
    // if not unwinding, stage these infos for inclusion into the base frame
    mm_assure(&xunit_infos_staging, &xunit_infos_stagingl, &xunit_infos_staginga, sizeof(*xunit_infos_staging), xunit_infos_stagingl + 1);

    // displace
    memmove(
        xunit_infos_staging + 1
      , xunit_infos_staging
      , xunit_infos_stagingl * sizeof(*xunit_infos_staging)
    );

    info * i = &xunit_infos_staging[0];
    xunit_infos_stagingl++;

    info_setvf(i, key, keyl, vfmt, va);
  }
}

void API xunit_info_unshiftf(const char * restrict key, const char * restrict vfmt, ...)
{
  va_list va;
  va_start(va, vfmt);

  xunit_info_unshiftvf(key, vfmt, va);

  va_end(va);
}
