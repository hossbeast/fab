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

#include "record.h"
#include "vtable.h"

#include "macros.h"
#include "common/assure.h"

//
// static
//

static __attribute__((nonnull)) xapi record_sayvf(narrator* const restrict N, const char * const restrict fmt, va_list va)
{
  enter;

  narrator_record *n = containerof(N, typeof(*n), base);

  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(n->s + n->l, n->a - n->l, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (n->a - n->l))
  {
    fatal(assure, &n->s, sizeof(*n->s), n->l + r + 1, &n->a);
    vsprintf(n->s + n->l, fmt, va);
  }
  n->l += r;

  finally : coda;
}

static __attribute__((nonnull)) xapi record_sayw(narrator* const restrict N, const void * const restrict b, size_t l)
{
  enter;

  narrator_record *n = containerof(N, typeof(*n), base);

  fatal(assure, &n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);
  n->s[n->l + l] = 0;
  n->l += l;

  finally : coda;
}

static __attribute__((nonnull)) xapi record_seek(narrator* const restrict N, off_t offset, int whence, off_t * restrict res)
{
  enter;

  narrator_record *n = containerof(N, typeof(*n), base);

  fatal(narrator_xseek, n->n, offset, whence, res);

  finally : coda;
}

static __attribute__((nonnull)) xapi record_read(narrator* restrict N, void * dst, size_t count, size_t * restrict r)
{
  enter;

  narrator_record *n = containerof(N, typeof(*n), base);

  xproxy(narrator_xread, n->n, dst, count, r);

  finally : coda;
}

static __attribute__((nonnull)) xapi record_flush(narrator* restrict N)
{
  enter;

  finally : coda;
}

struct narrator_vtable API narrator_record_vtable = NARRATOR_VTABLE(record);

//
// api
//

xapi API narrator_record_create(narrator_record ** const restrict rv, narrator * const restrict np)
{
  enter;

  narrator_record * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  n->base.vtab = &narrator_record_vtable;
  n->n = np;

  *rv = n;
  n = 0;

finally:
  fatal(narrator_record_free, n);
coda;
}

xapi API narrator_record_free(narrator_record * restrict n)
{
  enter;

  if(n) {
    fatal(narrator_record_destroy, n);
  }
  wfree(n);

  finally : coda;
}

xapi API narrator_record_destroy(narrator_record * restrict n)
{
  enter;

  wfree(n->s);

  finally : coda;
}

xapi API narrator_record_flush(narrator_record * const restrict n)
{
  enter;

  // flush to the underlying narrator and reset
  fatal(narrator_xsayw, n->n, n->s, n->l);
  n->l = 0;

  finally : coda;
}
