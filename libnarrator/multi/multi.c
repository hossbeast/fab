/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

    narrator_sayvf(n->v[x], fmt, *vap);

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
    narrator_sayw(n->v[x], b, l);
  }
}

static __attribute__((nonnull)) void multi_seek(narrator* const restrict N, off_t offset, int whence, off_t * restrict res)
{
  int x;
  narrator_multi *n = containerof(N, typeof(*n), base);

  for(x = 0; x < n->l; x++)
  {
    narrator_seek(n->v[x], offset, whence, res);
  }
}

static __attribute__((nonnull)) void multi_destroy(narrator* const restrict N)
{
  narrator_multi *n = containerof(N, typeof(*n), base);

  free(n->v);
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
  free(n);
}

void API narrator_multi_add(narrator_multi * const restrict n, narrator * const restrict np)
{
  assure(&n->v, sizeof(*n->v), n->l + 1, &n->a);
  n->v[n->l++] = np;
}
