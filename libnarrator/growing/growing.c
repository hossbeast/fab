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

#include "internal.h"
#include "growing.internal.h"

#include "macros.h"
#include "assure.h"

//
// public
//

xapi growing_xsayvf(narrator_growing * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

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
    fatal(assure, &n->s, sizeof(*n->s), n->l + r + 1, &n->a);
    vsprintf(n->s + n->l, fmt, va);
  }

  n->l += r;
  n->m = MAX(n->l, n->m);

  finally : coda;
}

xapi growing_xsayw(narrator_growing * const restrict n, const void * const restrict b, size_t l)
{
  enter;

  fatal(assure, &n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);

  n->l += l;
  n->m = MAX(n->l, n->m);

  finally : coda;
}

void growing_destroy(narrator_growing * const restrict n)
{
  wfree(n->s);
}

off_t growing_seek(narrator_growing * restrict n, off_t offset, int whence)
{
  if(whence == NARRATOR_SEEK_SET)
    n->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    n->l += offset;

  // for a growing narrator, the "end" is the greatest position ever written to
  else if(whence == NARRATOR_SEEK_END)
    n->l = (n->m + offset);

  return n->l;
}

off_t growing_reset(narrator_growing * restrict n)
{
  return n->m = n->l = 0;
}

size_t growing_read(narrator_growing * restrict n, void * dst, size_t count)
{
  size_t d = MIN(count, n->m - n->l);
  memcpy(dst, n->s + n->l, d);
  n->l += d;
  return d;
}

xapi growing_flush(narrator_growing * restrict n)
{
  enter;

  fatal(assure, &n->s, sizeof(*n->s), n->l + 1, &n->a);
  n->s[n->l] = 0;

  finally : coda;
}

//
// api
//

API xapi narrator_growing_create(narrator ** const restrict rv)
{
  enter;

  narrator * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  n->type = NARRATOR_GROWING;
  fatal(xmalloc, &n->growing.s, 1);

  *rv = n;
  n = 0;

finally:
  fatal(narrator_xfree, n);
coda;
}

API narrator * narrator_growing_init_from(char stor[NARRATOR_STATIC_SIZE], char * buf, size_t bufa)
{
  narrator * n = (void*)stor;
  n->type = NARRATOR_GROWING;
  memset(&n->growing, 0, sizeof(n->growing));
  n->growing.s = buf;
  n->growing.a = bufa;
  return n;
}

API narrator * narrator_growing_init(char stor[NARRATOR_STATIC_SIZE])
{
  return narrator_growing_init_from(stor, 0, 0);
}

API char * narrator_growing_buffer(narrator * const restrict n)
{
  return n->growing.s;
}

API void narrator_growing_claim_buffer(narrator * const restrict n, void * bufp, size_t * allocp)
{
  char * s = n->growing.s;
  size_t a = n->growing.a;

  if(bufp)
    *(void**)bufp = s;

  if(allocp)
    *allocp = a;

  memset(&n->growing, 0, sizeof(n->growing));
}

API size_t narrator_growing_size(narrator * const restrict n)
{
  return n->growing.l;
}

API xapi narrator_growing_allocate(narrator * restrict _n, size_t size)
{
  enter;

  narrator_growing * n = &_n->growing;

  fatal(assure, &n->s, sizeof(*n->s), n->l + size + 1, &n->a);

  finally : coda;
}
