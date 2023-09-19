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

#include "xlinux/xstdlib.h"

#include "strutil.internal.h"

//
// api
//


void API strloadc(char ** restrict dst, int c)
{
  strloadw(dst, (char[]) { c }, 1);
}

void API strloads(char ** restrict dst, const char * restrict s)
{
  strloadw(dst, s, strlen(s));
}

void API strloadw(char ** restrict dst, const void * restrict buf, size_t bufl)
{
  wfree(*dst);
  xmalloc(dst, bufl + 1);
  memcpy(*dst, buf, bufl);
  (*dst)[bufl] = 0;
}

void API strloadf(char ** restrict dst, const char * restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  strloadvf(dst, fmt, va);

  va_end(va);
}

void API strloadvf(char ** restrict dst, const char * restrict fmt, va_list va)
{
  va_list va2;
  va_copy(va2, va);

  int r = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  wfree(*dst);
  xmalloc(dst, r + 1);
  vsprintf(*dst, fmt, va);

  va_end(va);
}
