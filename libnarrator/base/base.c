/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "vtable.h"

//
// api
//

void API narrator_seek(narrator * const restrict n, off_t offset, int whence, off_t * restrict resp)
{
  n->vtab->seek(n, offset, whence, resp);
}

void API narrator_reset(narrator * const restrict n)
{
  n->vtab->seek(n, 0, SEEK_SET, 0);
  n->off = 0;
}

int API narrator_sayf(narrator * n, const char * const restrict fmt, ...)
{
  va_list va;
  int r;

  va_start(va, fmt);
  r = n->vtab->sayvf(n, fmt, va);
  va_end(va);

  n->off += r;

  return r;
}

int API narrator_sayc(narrator * const restrict n, int c)
{
  char cc = c;
  int r;

  r = n->vtab->sayw(n, &cc, 1);
  n->off += r;

  return r;
}

int API narrator_sayvf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  int r;

  r = n->vtab->sayvf(n, fmt, va);
  n->off += r;

  return r;
}

int API narrator_sayw(narrator * const restrict n, const void * const restrict b, size_t l)
{
  int r;

  r = n->vtab->sayw(n, b, l);
  n->off += r;

  return r;
}

int API narrator_says(narrator * const restrict n, const char * const restrict s)
{
  int r;

  if(!s) {
    return 0;
  }

  r = n->vtab->sayw(n, s, strlen(s));
  n->off += r;

  return r;
}

void API narrator_read(narrator * restrict n, void * dst, size_t count, size_t * restrict r)
{
  n->vtab->read(n, dst, count, r);
}

void API narrator_flush(narrator * restrict n)
{
  n->vtab->flush(n);
}
