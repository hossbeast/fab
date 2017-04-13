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

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "multi.internal.h"

#include "macros.h"
#include "assure.h"

#define restrict __restrict

//
// public
//

xapi multi_vsayf(narrator_multi * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_list * vap = 0;

  int x;
  for(x = 0; x < n->l; x++)
  {
    va_copy(va2, va);
    vap = &va2;

    fatal(narrator_vsayf, n->v[x], fmt, *vap);

    va_end(*vap);
    vap = 0;
  }

finally:
  if(vap)
    va_end(*vap);
coda;
}

xapi multi_sayw(narrator_multi * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  int x;
  for(x = 0; x < n->l; x++)
    fatal(narrator_sayw, n->v[x], b, l);

  finally : coda;
}

xapi multi_seek(narrator_multi * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  enter;

  int x;
  for(x = 0; x < n->l; x++)
    fatal(narrator_seek, n->v[x], offset, whence, res);

  finally : coda;
}

void multi_destroy(narrator_multi * const restrict n)
{
  wfree(n->v);
}

xapi multi_read(narrator_multi * restrict n, void * dst, size_t count)
{
  enter;
  // not supported
  finally : coda;
}

//
// api
//

API xapi narrator_multi_create(narrator ** const restrict rv)
{
  enter;

  narrator * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  n->type = NARRATOR_MULTI;
  fatal(xmalloc, &n->multi.v, sizeof(*n->multi.v));

  *rv = n;
  n = 0;

finally:
  fatal(narrator_xfree, n);
coda;
}

API xapi narrator_multi_add(narrator * const restrict n, narrator * const restrict np)
{
  enter;

  fatal(assure, &n->multi.v, sizeof(*n->multi.v), n->multi.l + 1, &n->multi.a);
  n->multi.v[n->multi.l++] = np;

  finally : coda;
}

API void narrator_multi_reset(narrator * const restrict n)
{
  n->multi.l = 0;
}
