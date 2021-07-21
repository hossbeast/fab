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

#include "xlinux/xstdlib.h"

#include "growing.internal.h"
#include "vtable.h"

#include "macros.h"
#include "common/assure.h"

//
// static
//

static void __attribute__((nonnull)) growing_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  narrator_growing *n = containerof(N, typeof(*n), base);
  va_list va2;
  va_copy(va2, va);
  int r;
  size_t sz;

  sz = 0;
  if(n->a > n->l)
    sz = n->a - n->l;

  // attempt to perform the entire write to the buffer
  r = vsnprintf(n->s + n->l, sz, fmt, va2);

  va_end(va2);

  // check whether output was truncated
  if(r >= sz)
  {
    assure(&n->s, sizeof(*n->s), n->l + r + 1, &n->a);
    vsprintf(n->s + n->l, fmt, va);
  }

  n->l += r;
  n->m = MAX(n->l, n->m);
}

static void __attribute__((nonnull)) growing_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  narrator_growing *n = containerof(N, typeof(*n), base);

  assure(&n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);

  n->l += l;
  n->m = MAX(n->l, n->m);
}

static void __attribute__((nonnull(1))) growing_seek(narrator * restrict N, off_t offset, int whence, off_t * restrict res)
{
  narrator_growing *n = containerof(N, typeof(*n), base);

  if(whence == NARRATOR_SEEK_SET)
    n->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    n->l += offset;

  // for a growing narrator, the "end" is the greatest position ever written to
  else if(whence == NARRATOR_SEEK_END)
    n->l = (n->m + offset);

  if(res)
  {
    *res = n->l;
  }
}

static void __attribute__((nonnull(1, 2))) growing_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{
  narrator_growing *n = containerof(N, typeof(*n), base);
  size_t d;

  d = MIN(count, n->m - n->l);
  memcpy(dst, n->s + n->l, d);
  n->l += d;
  if(r) {
    *r = d;
  }
}

static void __attribute__((nonnull)) growing_flush(narrator * restrict N)
{
  narrator_growing *n = containerof(N, typeof(*n), base);

  assure(&n->s, sizeof(*n->s), n->l + 1, &n->a);
  n->s[n->l] = 0;
}

static struct narrator_vtable growing_vtable = NARRATOR_VTABLE(growing);

//
// public
//

//
// api
//

void API narrator_growing_create(narrator_growing ** const restrict rv)
{
  narrator_growing * n = 0;

  xmalloc(&n, sizeof(*n));

  n->base.vtab = &growing_vtable;
  xmalloc(&n->s, 1);

  *rv = n;
  n = 0;

  narrator_growing_free(n);
}

void API narrator_growing_free(narrator_growing * restrict n)
{
  if(n) {
    narrator_growing_destroy(n);
  }
  wfree(n);
}

narrator * API narrator_growing_init_from(narrator_growing * restrict n, char * buf, size_t bufa)
{
  n->base.vtab = &growing_vtable;
  n->s = buf;
  n->l = 0;
  n->a = bufa;
  n->m = 0;

  return &n->base;
}

narrator * API narrator_growing_init(narrator_growing * restrict n)
{
  return narrator_growing_init_from(n, 0, 0);
}

void API narrator_growing_destroy(narrator_growing * restrict n)
{
  wfree(n->s);
}

void API narrator_growing_claim_buffer(narrator_growing * const restrict n, void * bufp, size_t * allocp)
{
  char * s = n->s;
  size_t a = n->a;

  if(bufp)
    *(void**)bufp = s;

  if(allocp)
    *allocp = a;

  n->s = 0;
  n->l = 0;
  n->a = 0;
  n->m = 0;
}

void API narrator_growing_allocate(narrator_growing * restrict n, size_t size)
{
  assure(&n->s, sizeof(*n->s), n->l + size + 1, &n->a);
}
