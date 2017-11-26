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
#include <errno.h>

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

  if(n && n->type == NARRATOR_FIXED)
    goto XAPI_FINALIZE;

  if(n)
  {
    // dispatch
    if(n->type == NARRATOR_FD)
      fd_destroy(&n->fd);
    else if(n->type == NARRATOR_GROWING)
      growing_destroy(&n->growing);
    else if(n->type == NARRATOR_MULTI)
      multi_destroy(&n->multi);
    else if(n->type == NARRATOR_NULLITY)
      nullity_destroy(&n->nullity);
    else if(n->type == NARRATOR_RECORD)
      record_destroy(&n->record);
    else if(n->type == NARRATOR_ROLLING)
      fatal(rolling_xdestroy, &n->rolling);
    else
      fail(SYS_NOTSUPP);

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

API xapi narrator_xseek(narrator * const restrict n, off_t offset, int whence, off_t * restrict resp)
{
  enter;

  off_t res = 0;

  if(n->type == NARRATOR_FD)
    fatal(fd_xseek, &n->fd, offset, whence, &res);
  else if(n->type == NARRATOR_MULTI)
    fatal(multi_xseek, &n->multi, offset, whence, &res);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_xseek, &n->record, offset, whence, &res);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_xseek, &n->rolling, offset, whence, &res);

  else if(n->type == NARRATOR_FIXED)
    res = fixed_seek(&n->fixed, offset, whence);
  else if(n->type == NARRATOR_GROWING)
    res = growing_seek(&n->growing, offset, whence);
  else if(n->type == NARRATOR_NULLITY)
    res = nullity_seek(&n->nullity, offset, whence);

  else
    fail(SYS_NOTSUPP);

  if(resp)
    *resp = res;

  finally : coda;
}

API xapi narrator_xreset(narrator * const restrict n)
{
  xproxy(narrator_xseek, n, 0, SEEK_SET, 0);
}

API xapi narrator_xsayf(narrator * n, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fatal(narrator_xsayvf, n, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi narrator_xsayc(narrator * const restrict n, int c)
{
  xproxy(narrator_xsayw, n, (char*)&c, 1);
}

API xapi narrator_xsayvf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  if(n->type == NARRATOR_FD)
    fatal(fd_xsayvf, &n->fd, fmt, va);
  else if(n->type == NARRATOR_GROWING)
    fatal(growing_xsayvf, &n->growing, fmt, va);
  else if(n->type == NARRATOR_MULTI)
    fatal(multi_xsayvf, &n->multi, fmt, va);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_xsayvf, &n->record, fmt, va);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_xsayvf, &n->rolling, fmt, va);

  else if(n->type == NARRATOR_FIXED)
    fixed_sayvf(&n->fixed, fmt, va);
  else if(n->type == NARRATOR_NULLITY)
    nullity_sayvf(&n->nullity, fmt, va);

  else
    fail(SYS_NOTSUPP);

  finally : coda;
}

API xapi narrator_xsayw(narrator * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  if(n->type == NARRATOR_FD)
    fatal(fd_xsayw, &n->fd, b, l);
  else if(n->type == NARRATOR_GROWING)
    fatal(growing_xsayw, &n->growing, b, l);
  else if(n->type == NARRATOR_MULTI)
    fatal(multi_xsayw, &n->multi, b, l);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_xsayw, &n->record, b, l);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_xsayw, &n->rolling, b, l);

  else if(n->type == NARRATOR_FIXED)
    fixed_sayw(&n->fixed, b, l);
  else if(n->type == NARRATOR_NULLITY)
    nullity_sayw(&n->nullity, b, l);

  else
    fail(SYS_NOTSUPP);

  finally : coda;
}

API xapi narrator_xsays(narrator * const restrict n, const char * const restrict s)
{
  xproxy(narrator_xsayw, n, s, strlen(s));
}

API xapi narrator_xread(narrator * restrict n, void * dst, size_t count)
{
  enter;

  if(n->type == NARRATOR_FD)
    fatal(fd_xread, &n->fd, dst, count);
  else if(n->type == NARRATOR_ROLLING)
    fatal(rolling_xread, &n->rolling, dst, count);
  else if(n->type == NARRATOR_RECORD)
    fatal(record_xread, &n->record, dst, count);
  else if(n->type == NARRATOR_FIXED)
    fixed_read(&n->fixed, dst, count);
  else if(n->type == NARRATOR_GROWING)
    growing_read(&n->growing, dst, count);
  else if(n->type == NARRATOR_NULLITY)
    nullity_read(&n->nullity, dst, count);

  else
    fail(SYS_NOTSUPP);

  finally : coda;
}
