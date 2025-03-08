/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "fixed.h"
#include "vtable.h"

#include "macros.h"
#include "zcat.h"

//
// static
//

static int __attribute__((nonnull)) fixed_sayvf(narrator* const restrict N, const char * const restrict fmt, va_list va)
{
  narrator_fixed *n = containerof(N, typeof(*n), base);
  size_t l;

  l = zcatvf(n->s + n->l, n->a - n->l, fmt, va);
  n->l += l;
  n->m = MAX(n->l, n->m);

  return l;
}

static int __attribute__((nonnull)) fixed_sayw(narrator* const restrict N, const void * const restrict b, size_t l)
{
  narrator_fixed *n = containerof(N, typeof(*n), base);

  n->l += zcatw(n->s + n->l, n->a - n->l, b, l);
  n->m = MAX(n->l, n->m);

  return l;
}

static void __attribute__((nonnull(1))) fixed_seek(narrator * const restrict N, off_t offset, int whence, off_t *res)
{
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
}

static void __attribute__((nonnull(1, 2))) fixed_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{
  narrator_fixed *n = containerof(N, typeof(*n), base);
  size_t d;

  d = MIN(count, n->m - n->l);
  memcpy(dst, n->s + n->l, d);
  n->l += d;
  if(r) {
    *r = d;
  }
}

static void __attribute__((nonnull)) fixed_flush(narrator * restrict N)
{
  narrator_fixed *n = containerof(N, typeof(*n), base);

  n->s[n->l] = 0;
}

static struct narrator_vtable fixed_vtable = NARRATOR_VTABLE(fixed);

//
// api
//

narrator *narrator_fixed_init(narrator_fixed * restrict n, char * s, size_t a)
{
  n->base.vtab = &fixed_vtable;
  n->s = s;
  n->l = 0;
  n->a = a;
  n->m = 0;
  n->base.off = 0;

  return &n->base;
}

void narrator_fixed_destroy(narrator_fixed * restrict n)
{

}
