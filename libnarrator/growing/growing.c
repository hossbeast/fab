/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "growing.internal.h"
#include "vtable.h"

#include "macros.h"
#include "common/assure.h"
#include "zcat.h"

//
// static
//

static int __attribute__((nonnull)) growing_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  narrator_growing *n = containerof(N, typeof(*n), base);
  va_list va2;
  int r;

  RUNTIME_ASSERT(n->a >= n->l);

  // attempt to perform the entire write to the buffer
  va_copy(va2, va);
  r = vsnprintf(n->s + n->l, n->a - n->l, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (n->a - n->l))
  {
    assure(&n->s, sizeof(*n->s), n->l + r + 1, &n->a);
    r = vsnprintf(n->s + n->l, n->a - n->l, fmt, va);
    va_end(va);
  }

  n->l += r;
  n->m = MAX(n->l, n->m);

  return r;
}

static int __attribute__((nonnull)) growing_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  narrator_growing *n = containerof(N, typeof(*n), base);

  assure(&n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);

  n->l += l;
  n->m = MAX(n->l, n->m);

  return l;
}

static void __attribute__((nonnull(1))) growing_seek(narrator * restrict N, off_t offset, int whence, off_t * restrict res)
{
  narrator_growing *n = containerof(N, typeof(*n), base);

  if(whence == NARRATOR_SEEK_SET) {
    n->l = offset;
  } else if(whence == NARRATOR_SEEK_CUR) {
    n->l += offset;
  }

  // for a growing narrator, the "end" is the greatest position ever written to
  else if(whence == NARRATOR_SEEK_END) {
    n->l = (n->m + offset);
  }

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
  narrator_growing_init(n);

  *rv = n;
}

void API narrator_growing_free(narrator_growing * restrict n)
{
  if(n) {
    narrator_growing_destroy(n);
  }
  free(n);
}

narrator * API narrator_growing_init_from(narrator_growing * restrict n, char * buf, size_t bufa)
{
  narrator_growing_init(n);

  n->s = buf;
  n->a = bufa;

  return &n->base;
}

narrator * API narrator_growing_init(narrator_growing * restrict n)
{
  n->base.vtab = &growing_vtable;
  n->s = 0;
  n->l = 0;
  n->a = 0;
  n->m = 0;

  return &n->base;
}

void API narrator_growing_destroy(narrator_growing * restrict n)
{
  free(n->s);
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
