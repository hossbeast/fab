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

#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "multi.h"
#include "vtable.h"

#include "macros.h"
#include "common/assure.h"

//
// static
//

static __attribute__((nonnull)) void multi_sayvf(narrator* const restrict N, const char * const restrict fmt, va_list va)
{
  narrator_multi *n = containerof(N, typeof(*n), base);

  va_list va2;
  va_list * vap = 0;
  int x;

  for(x = 0; x < n->l; x++)
  {
    va_copy(va2, va);
    vap = &va2;

    narrator_xsayvf(n->v[x], fmt, *vap);

    va_end(*vap);
    vap = 0;
  }

  if(vap) {
    va_end(*vap);
  }
}

static __attribute__((nonnull)) void multi_sayw(narrator* const restrict N, const void * const restrict b, size_t l)
{
  narrator_multi *n = containerof(N, typeof(*n), base);
  int x;

  for(x = 0; x < n->l; x++)
  {
    narrator_xsayw(n->v[x], b, l);
  }
}

static __attribute__((nonnull)) void multi_seek(narrator* const restrict N, off_t offset, int whence, off_t * restrict res)
{
  int x;
  narrator_multi *n = containerof(N, typeof(*n), base);

  for(x = 0; x < n->l; x++)
  {
    narrator_xseek(n->v[x], offset, whence, res);
  }
}

static __attribute__((nonnull)) void multi_destroy(narrator* const restrict N)
{
  narrator_multi *n = containerof(N, typeof(*n), base);

  wfree(n->v);
}

/* not implemented */
#define multi_read 0
#define multi_flush 0
static struct narrator_vtable multi_vtable = NARRATOR_VTABLE(multi);

//
// public
//

//
// api
//

void API narrator_multi_create(narrator_multi ** const restrict rv)
{
  narrator_multi * n = 0;

  xmalloc(&n, sizeof(*n));

  n->base.vtab = &multi_vtable;
  xmalloc(&n->v, sizeof(*n->v));

  *rv = n;
  n = 0;

  narrator_multi_free(n);
}

void API narrator_multi_free(narrator_multi * restrict n)
{
  if(n) {
    multi_destroy(&n->base);
  }
  wfree(n);
}

void API narrator_multi_add(narrator_multi * const restrict n, narrator * const restrict np)
{
  assure(&n->v, sizeof(*n->v), n->l + 1, &n->a);
  n->v[n->l++] = np;
}
