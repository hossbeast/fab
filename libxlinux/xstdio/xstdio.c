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
#include <stdarg.h>
#include <errno.h>

#include "xapi.h"
#include "types.h"

#include "internal.h"
#include "xstdio/xstdio.h"
#include "errtab/KERNEL.errtab.h"

#include "fmt.h"

API xapi xvdprintf(int fd, const char * const restrict fmt, va_list va)
{
  enter;

  if(vdprintf(fd, fmt, va) < 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}

API xapi xrenames(const char * const restrict old, const char * const restrict new)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, rename, old, new);

finally:
  xapi_infof("old", "%s", old);
  xapi_infof("new", "%s", new);
coda;
}

API xapi xrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
{
  enter;

  va_list va;
  va_start(va, newfmt);

  fatal(xrenamevf, oldfmt, newfmt, va);

finally:
  va_end(va);
coda;
}

API xapi xrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
{
  enter;

  char old[512];
  char new[512];
  fatal(fmt_apply, old, sizeof(old), oldfmt, va);
  fatal(fmt_apply, new, sizeof(new), newfmt, va);

  fatal(xrenames, old, new);

  finally : coda;
}

API xapi uxrenames(const char * const restrict old, const char * const restrict new)
{
  enter;

  if(rename(old, new) != 0 && errno != ENOENT && errno != ENOTDIR)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("old", "%s", old);
  xapi_infof("new", "%s", new);
coda;
}

API xapi uxrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
{
  enter;

  va_list va;
  va_start(va, newfmt);

  fatal(uxrenamevf, oldfmt, newfmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
{
  enter;

  char old[512];
  char new[512];
  fatal(fmt_apply, old, sizeof(old), oldfmt, va);
  fatal(fmt_apply, new, sizeof(new), newfmt, va);

  fatal(uxrenames, old, new);

  finally : coda;
}
