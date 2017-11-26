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

#include <stdlib.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "nullity.internal.h"

APIDATA narrator * g_narrator_nullity = 0;

//
// public
//

xapi nullity_setup()
{
  enter;

  fatal(narrator_nullity_create, &g_narrator_nullity);

  finally : coda;
}

xapi nullity_cleanup()
{
  xproxy(narrator_xfree, g_narrator_nullity);
}

int nullity_sayvf(narrator_nullity * const restrict n, const char * const restrict fmt, va_list va)
{
  return 0;
}

int nullity_sayw(narrator_nullity * const restrict n, const char * const restrict b, size_t l)
{
  return 0;
}

off_t nullity_seek(narrator_nullity * const restrict n, off_t offset, int whence)
{
  return 0;
}

void nullity_destroy(narrator_nullity * const restrict n)
{
}

int nullity_read(narrator_nullity * restrict n, void * dst, size_t count)
{
  return 0;
}

//
// api
//

API xapi narrator_nullity_create(narrator ** const restrict rv)
{
  enter;

  fatal(xmalloc, rv, sizeof(**rv));
  (*rv)->type = NARRATOR_NULLITY;

  finally : coda;
}
