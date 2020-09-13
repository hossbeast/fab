/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/vtable.h"

#include "bacon_narrator.h"

#include "macros.h"
#include "common/assure.h"

//
// static
//

static __attribute__((nonnull)) xapi propagate(bacon_narrator * const restrict n, const char * s, size_t l)
{
  enter;

  int x;
  int y;

//printf("l %zu - %.*s - ", l, (int)l, s);

  /* propagate */
  x = 0;
  y = 0;
  for(; x < l; x++) {
    if(s[x] >= 0x20 && s[x] <= 0x7e) {
      continue;
    }

//printf(" x %d y %d", x, y);

    fatal(narrator_xsayw, n->n, &s[y], x - y);
    fatal(narrator_xsayf, n->n, "\\x%02hx", s[x]);
    y = x;
  }
//printf(" after - x %d y %d\n", x, y);
  fatal(narrator_xsayw, n->n, &s[y], x - y);

  finally : coda;
}

static __attribute__((nonnull)) xapi bacon_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  enter;

  bacon_narrator *n = containerof(N, typeof(*n), base);
  va_list va2;
  int r;

  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  r = vsnprintf(n->s, n->a, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= n->a)
  {
    fatal(assure, &n->s, sizeof(*n->s), r + 1, &n->a);
    vsprintf(n->s, fmt, va);
  }

  fatal(propagate, n, n->s, r);

  finally : coda;
}

static __attribute__((nonnull)) xapi bacon_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  enter;

  bacon_narrator *n = containerof(N, typeof(*n), base);

  fatal(propagate, n, b, l);

  finally : coda;
}

static __attribute__((nonnull)) xapi bacon_seek(narrator * const restrict N, off_t offset, int whence, off_t * restrict res)
{
  enter;

  bacon_narrator *n = containerof(N, typeof(*n), base);

  fatal(narrator_xseek, n->n, offset, whence, res);

  finally : coda;
}

#define bacon_read 0
#define bacon_flush 0
const static narrator_vtable bacon_vtable = NARRATOR_VTABLE(bacon);

//
// api
//

xapi API bacon_narrator_init(bacon_narrator * const restrict n, narrator * const restrict np)
{
  enter;

  n->base.vtab = &bacon_vtable;
  n->n = np;

  finally : coda;
}

xapi API bacon_narrator_create(bacon_narrator ** const restrict rv, narrator * const restrict np)
{
  enter;

  bacon_narrator * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  fatal(bacon_narrator_init, n, np);
  *rv = n;
  n = 0;

finally:
  fatal(bacon_narrator_free, n);
coda;
}

xapi API bacon_narrator_free(bacon_narrator * const restrict n)
{
  enter;

  if(n) {
    fatal(bacon_narrator_destroy, n);
  }
  wfree(n);

  finally : coda;
}

xapi API bacon_narrator_destroy(bacon_narrator * const restrict n)
{
  enter;

  wfree(n->s);

  finally : coda;
}
