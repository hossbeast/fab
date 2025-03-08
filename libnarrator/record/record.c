/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "record.h"
#include "vtable.h"

#include "macros.h"
#include "common/assure.h"

//
// static
//

static int __attribute__((nonnull)) record_sayvf(narrator* const restrict N, const char * const restrict fmt, va_list va)
{
  narrator_record *n = containerof(N, typeof(*n), base);

  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(n->s + n->l, n->a - n->l, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (n->a - n->l))
  {
    assure(&n->s, sizeof(*n->s), n->l + r + 1, &n->a);
    vsprintf(n->s + n->l, fmt, va);
  }
  n->l += r;

  return r;
}

static int __attribute__((nonnull)) record_sayw(narrator* const restrict N, const void * const restrict b, size_t l)
{
  narrator_record *n = containerof(N, typeof(*n), base);

  assure(&n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);
  n->s[n->l + l] = 0;
  n->l += l;

  return l;
}

static __attribute__((nonnull)) void record_seek(narrator* const restrict N, off_t offset, int whence, off_t * restrict res)
{
  narrator_record *n = containerof(N, typeof(*n), base);

  narrator_seek(n->n, offset, whence, res);
}

static __attribute__((nonnull)) void record_read(narrator* restrict N, void * dst, size_t count, size_t * restrict r)
{
  narrator_record *n = containerof(N, typeof(*n), base);

  narrator_read(n->n, dst, count, r);
}

static __attribute__((nonnull)) void record_flush(narrator* restrict N)
{

}

struct narrator_vtable narrator_record_vtable = NARRATOR_VTABLE(record);

//
// api
//

void narrator_record_create(narrator_record ** const restrict rv, narrator * const restrict np)
{
  narrator_record * n = 0;

  xmalloc(&n, sizeof(*n));

  n->base.vtab = &narrator_record_vtable;
  n->n = np;

  *rv = n;
  n = 0;

  narrator_record_free(n);
}

void narrator_record_free(narrator_record * restrict n)
{
  if(n) {
    narrator_record_destroy(n);
  }
  free(n);
}

void narrator_record_destroy(narrator_record * restrict n)
{
  free(n->s);
}

void narrator_record_flush(narrator_record * const restrict n)
{
  // flush to the underlying narrator and reset
  narrator_sayw(n->n, n->s, n->l);
  n->l = 0;
}
