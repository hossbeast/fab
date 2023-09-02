/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "xlinux/xstdlib.h"
#include "narrator/vtable.h"

#include "bacon_narrator.h"

#include "macros.h"
#include "common/assure.h"

//
// static
//

static __attribute__((nonnull)) void bacon_sayvf(narrator* const restrict N, const char * const restrict fmt, va_list va)
{
  bacon_narrator *n = containerof(N, typeof(*n), base);

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
}

static __attribute__((nonnull)) void bacon_sayw(narrator* const restrict N, const void * const restrict b, size_t l)
{
  bacon_narrator *n = containerof(N, typeof(*n), base);

  assure(&n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);
  n->s[n->l + l] = 0;
  n->l += l;
}

static __attribute__((nonnull)) void bacon_seek(narrator* const restrict N, off_t offset, int whence, off_t * restrict res)
{
  bacon_narrator *n = containerof(N, typeof(*n), base);

  narrator_seek(n->next, offset, whence, res);
}

static __attribute__((nonnull)) void bacon_read(narrator* restrict N, void * dst, size_t count, size_t * restrict r)
{
  bacon_narrator *n = containerof(N, typeof(*n), base);

  narrator_read(n->next, dst, count, r);
}

static __attribute__((nonnull)) void bacon_flush(narrator* restrict N)
{

}

struct narrator_vtable bacon_narrator_vtable = NARRATOR_VTABLE(bacon);

//
// api
//

narrator *bacon_narrator_init(bacon_narrator * const restrict n)
{
  memset(n, 0, sizeof(*n));
  n->base.vtab = &bacon_narrator_vtable;
  return &n->base;
}

void bacon_narrator_destroy(bacon_narrator * restrict n)
{
  free(n->s);
}

void bacon_narrator_configure(bacon_narrator * restrict n, narrator * restrict next)
{
  n->next = next;
}

void bacon_narrator_flush(bacon_narrator * const restrict n)
{
  // flush to the underlying narrator and reset
  narrator_sayw(n->next, n->s, n->l);
  n->l = 0;
}
