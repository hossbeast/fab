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
#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "vtable.h"

//
// api
//

xapi API narrator_xseek(narrator * const restrict n, off_t offset, int whence, off_t * restrict resp)
{
  enter;

  fatal(n->vtab->seek, n, offset, whence, resp);

  finally : coda;
}

xapi API narrator_xreset(narrator * const restrict n)
{
  enter;

  fatal(n->vtab->seek, n, 0, SEEK_SET, 0);

  finally : coda;
}

xapi API narrator_xsayf(narrator * n, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fatal(n->vtab->sayvf, n, fmt, va);

finally:
  va_end(va);
coda;
}

xapi API narrator_xsayc(narrator * const restrict n, int c)
{
  enter;

  char cc = c;

  fatal(n->vtab->sayw, n, &cc, 1);

  finally : coda;
}

xapi API narrator_xsayvf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  fatal(n->vtab->sayvf, n, fmt, va);

  finally : coda;
}

xapi API narrator_xsayw(narrator * const restrict n, const void * const restrict b, size_t l)
{
  enter;

  fatal(n->vtab->sayw, n, b, l);

  finally : coda;
}

xapi API narrator_xsays(narrator * const restrict n, const char * const restrict s)
{
  enter;

  if(s) {
    fatal(n->vtab->sayw, n, s, strlen(s));
  }

  finally : coda;
}

xapi API narrator_xread(narrator * restrict n, void * dst, size_t count, size_t * restrict r)
{
  enter;

  fatal(n->vtab->read, n, dst, count, r);

  finally : coda;
}

xapi API narrator_flush(narrator * restrict n)
{
  enter;

  fatal(n->vtab->flush, n);

  finally : coda;
}
