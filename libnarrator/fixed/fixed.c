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
#include <stdlib.h>

#include "types.h"
#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "vtable.h"
#include "fixed.h"

#include "zbuffer.h"

//
// static
//

static xapi __attribute__((nonnull)) fixed_sayvf(narrator* const restrict N, const char * const restrict fmt, va_list va)
{
  enter;

  narrator_fixed *n = containerof(N, typeof(*n), base);
  size_t l;

  l = znloadvf(n->s + n->l, n->a - n->l, fmt, va);
  n->l += l;
  n->m = MAX(n->l, n->m);

  finally : coda;
}

static xapi __attribute__((nonnull)) fixed_sayw(narrator* const restrict N, const void * const restrict b, size_t l)
{
  enter;

  narrator_fixed *n = containerof(N, typeof(*n), base);

  n->l += znloadw(n->s + n->l, n->a - n->l, b, l);
  n->m = MAX(n->l, n->m);

  finally : coda;
}

static xapi __attribute__((nonnull(1))) fixed_seek(narrator * const restrict N, off_t offset, int whence, off_t *res)
{
  enter;

  narrator_fixed *n = containerof(N, typeof(*n), base);

  if(whence == NARRATOR_SEEK_SET)
    n->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    n->l += offset;

  // for a fixed narrator, the "end" is the allocated size
  else if(whence == NARRATOR_SEEK_END)
    n->l = (n->a + offset);

  if(res) {
    *res = n->l;
  }

  finally : coda;
}

static xapi __attribute__((nonnull(1, 2))) fixed_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{
  enter;

  narrator_fixed *n = containerof(N, typeof(*n), base);
  size_t d;

  d = MIN(count, n->m - n->l);
  memcpy(dst, n->s + n->l, d);
  n->l += d;
  if(r) {
    *r = d;
  }

  finally : coda;
}

static xapi __attribute__((nonnull)) fixed_flush(narrator * restrict N)
{
  enter;

  narrator_fixed *n = containerof(N, typeof(*n), base);

  n->s[n->l] = 0;

  finally : coda;
}

static struct narrator_vtable fixed_vtable = NARRATOR_VTABLE(fixed);

//
// api
//

narrator * API narrator_fixed_init(narrator_fixed * restrict n, char * s, size_t a)
{
  n->base.vtab = &fixed_vtable;
  n->s = s;
  n->l = 0;
  n->a = a;
  n->m = 0;

  return &n->base;
}

xapi API narrator_fixed_destroy(narrator_fixed * restrict n)
{
  enter;

  finally : coda;
}
