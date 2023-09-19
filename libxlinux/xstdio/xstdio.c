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

#include "types.h"

#include "xstdio/xstdio.h"

#include "common/fmt.h"

void API xvdprintf(int fd, const char * const restrict fmt, va_list va)
{
  RUNTIME_ASSERT(vdprintf(fd, fmt, va) >= 0);
}

void API xrenames(const char * const restrict old, const char * const restrict new)
{
  RUNTIME_ASSERT(rename(old, new) == 0);
}

void API xrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
{
  va_list va;

  va_start(va, newfmt);
  xrenamevf(oldfmt, newfmt, va);
  va_end(va);
}

void API xrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
{
  char old[512];
  char new[512];

  fmt_apply(old, sizeof(old), oldfmt, va);
  fmt_apply(new, sizeof(new), newfmt, va);

  xrenames(old, new);
}

void API uxrenames(const char * const restrict old, const char * const restrict new)
{
  int r;

  r = rename(old, new);
  RUNTIME_ASSERT(r == 0 || errno == ENOENT || errno == ENOTDIR);
}

void API uxrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
{
  va_list va;

  va_start(va, newfmt);
  uxrenamevf(oldfmt, newfmt, va);
  va_end(va);
}

void API uxrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
{
  char old[512];
  char new[512];

  fmt_apply(old, sizeof(old), oldfmt, va);
  fmt_apply(new, sizeof(new), newfmt, va);

  uxrenames(old, new);
}
