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

#include "xapi.h"
#include "xapi/SYS.errtab.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "fd.internal.h"
#include "fixed.internal.h"
#include "growing.internal.h"
#include "multi.internal.h"
#include "nullity.internal.h"
#include "record.internal.h"
#include "rolling.internal.h"

//
// api
//

API xapi narrator_xfree(narrator * restrict n)
{
  enter;

  if(n)
  {
    // dispatch
    if(n->type == NARRATOR_GROWING)
      growing_destroy(&n->growing);
    else if(n->type == NARRATOR_FIXED)
      fixed_destroy(&n->fixed);
    else if(n->type == NARRATOR_FD)
      fd_destroy(&n->fd);
    else if(n->type == NARRATOR_MULTI)
      multi_destroy(&n->multi);
    else if(n->type == NARRATOR_NULLITY)
      nullity_destroy(&n->nullity);
    else if(n->type == NARRATOR_RECORD)
      record_destroy(&n->record);
    else if(n->type == NARRATOR_ROLLING)
      fatal(rolling_xdestroy, &n->rolling);

    wfree(n);
  }

  finally : coda;
}

API xapi narrator_ixfree(narrator ** const restrict n)
{
  enter;

  fatal(narrator_xfree, *n);
  *n = 0;

  finally : coda;
}

API xapi narrator_seek(narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  enter;

  // dispatch
  if(n->type == NARRATOR_GROWING)
    fatal(growing_seek, &n->growing, offset, whence, res);
  else if(n->type == NARRATOR_FIXED)
    fatal(fixed_seek, &n->fixed, offset, whence, res);
  else if(n->type == NARRATOR_FD)
    fatal(fd_seek, &n->fd, offset, whence, res);
  else if(n->type == NARRATOR_MULTI)
    fatal(multi_seek, &n->multi, offset, whence, res);
  else if(n->type == NARRATOR_NULLITY)
    fatal(nullity_seek, &n->nullity, offset, whence, res);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_seek, &n->record, offset, whence, res);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_seek, &n->rolling, offset, whence, res);

  finally : coda;
}

API xapi narrator_reset(narrator * const restrict n)
{
  xproxy(narrator_seek, n, 0, SEEK_SET, 0);
}

API xapi narrator_sayf(narrator * n, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fatal(narrator_vsayf, n, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi narrator_sayc(narrator * const restrict n, int c)
{
  xproxy(narrator_sayw, n, (char*)&c, 1);
}

API xapi narrator_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  if(n->type == NARRATOR_GROWING)
    fatal(growing_vsayf, &n->growing, fmt, va);
  else if(n->type == NARRATOR_FIXED)
    fatal(fixed_vsayf, &n->fixed, fmt, va);
  else if(n->type == NARRATOR_FD)
    fatal(fd_vsayf, &n->fd, fmt, va);
  else if(n->type == NARRATOR_MULTI)
    fatal(multi_vsayf, &n->multi, fmt, va);
  else if(n->type == NARRATOR_NULLITY)
    fatal(nullity_vsayf, &n->nullity, fmt, va);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_vsayf, &n->record, fmt, va);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_vsayf, &n->rolling, fmt, va);

  finally : coda;
}

API xapi narrator_sayw(narrator * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  if(n->type == NARRATOR_GROWING)
    fatal(growing_sayw, &n->growing, b, l);
  else if(n->type == NARRATOR_FIXED)
    fatal(fixed_sayw, &n->fixed, b, l);
  else if(n->type == NARRATOR_FD)
    fatal(fd_sayw, &n->fd, b, l);
  else if(n->type == NARRATOR_MULTI)
    fatal(multi_sayw, &n->multi, b, l);
  else if(n->type == NARRATOR_NULLITY)
    fatal(nullity_sayw, &n->nullity, b, l);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_sayw, &n->record, b, l);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_sayw, &n->rolling, b, l);

  finally : coda;
}

API xapi narrator_says(narrator * const restrict n, const char * const restrict s)
{
  xproxy(narrator_sayw, n, s, strlen(s));
}
