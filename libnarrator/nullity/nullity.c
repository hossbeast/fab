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
#include "xlinux.h"

#include "internal.h"
#include "nullity.internal.h"

#define restrict __restrict

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

void nullity_teardown()
{
  narrator_free(g_narrator_nullity);
}

xapi nullity_vsayf(narrator_nullity * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  finally : coda;
}

xapi nullity_sayw(narrator_nullity * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  finally : coda;
}

xapi nullity_seek(narrator_nullity * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  enter;

  finally : coda;
}

void nullity_destroy(narrator_nullity * n)
{
}

//
// api
//

API xapi narrator_nullity_create(narrator ** const restrict rv)
{
  enter;

  narrator * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  n->type = NARRATOR_NULLITY;
  
  *rv = n;
  n = 0;

finally:
  narrator_free(n);
coda;
}
