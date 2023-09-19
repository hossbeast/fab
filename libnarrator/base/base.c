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

#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "vtable.h"

//
// api
//

void API narrator_xseek(narrator * const restrict n, off_t offset, int whence, off_t * restrict resp)
{
  n->vtab->seek(n, offset, whence, resp);
}

void API narrator_xreset(narrator * const restrict n)
{
  n->vtab->seek(n, 0, SEEK_SET, 0);
}

void API narrator_xsayf(narrator * n, const char * const restrict fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  n->vtab->sayvf(n, fmt, va);
  va_end(va);
}

void API narrator_xsayc(narrator * const restrict n, int c)
{
  char cc = c;

  n->vtab->sayw(n, &cc, 1);
}

void API narrator_xsayvf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  n->vtab->sayvf(n, fmt, va);
}

void API narrator_xsayw(narrator * const restrict n, const void * const restrict b, size_t l)
{
  n->vtab->sayw(n, b, l);
}

void API narrator_xsays(narrator * const restrict n, const char * const restrict s)
{
  if(!s) {
    return;
  }

  n->vtab->sayw(n, s, strlen(s));
}

void API narrator_xread(narrator * restrict n, void * dst, size_t count, size_t * restrict r)
{
  n->vtab->read(n, dst, count, r);
}

void API narrator_flush(narrator * restrict n)
{
  n->vtab->flush(n);
}
