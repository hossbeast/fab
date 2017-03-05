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

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "strutil.internal.h"

#define restrict __restrict

//
// api
//


API xapi strloadc(char ** restrict dst, int c)
{
  xproxy(strloadw, dst, (char[]) { c }, 1);
}

API xapi strloads(char ** restrict dst, const char * restrict s)
{
  xproxy(strloadw, dst, s, strlen(s));
}

API xapi strloadw(char ** restrict dst, const void * restrict buf, size_t bufl)
{
  enter;

  wfree(*dst);
  fatal(xmalloc, dst, bufl + 1);
  memcpy(*dst, buf, bufl);
  ((char*)buf)[bufl] = 0;

  finally : coda;
}

API xapi strloadf(char ** restrict dst, const char * restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fatal(strloadvf, dst, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi strloadvf(char ** restrict dst, const char * restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  int r = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  wfree(*dst);
  fatal(xmalloc, dst, r + 1);
  vsprintf(*dst, fmt, va);

finally:
  va_end(va);
coda;
}
