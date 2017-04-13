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

#define restrict __restrict

//
// public
//

xapi growing_vsayf(narrator_growing * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(n->s + n->l, n->a - n->l, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (n->a - n->l))
  {
    fatal(assure, &n->s, sizeof(*n->s), n->l + r + 1, &n->a);
    vsprintf(n->s + n->l, fmt, va);
  }
  n->l += r;
  n->m = MAX(n->l, n->m);

  finally : coda;
}

xapi growing_sayw(narrator_growing * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  fatal(assure, &n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);
  n->s[n->l + l] = 0;
  n->l += l;
  n->m = MAX(n->l, n->m);

  finally : coda;
}

void growing_destroy(narrator_growing * const restrict n)
{
  wfree(n->s);
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

API const char * narrator_growing_buffer(narrator * const restrict n)
{
  return n->growing.s;
}

API size_t narrator_growing_size(narrator * const restrict n)
{
  return n->growing.l;
}

API off_t narrator_growing_seek(narrator * restrict n, off_t offset, int whence)
{
  narrator_growing * g = &n->growing;

  if(whence == NARRATOR_SEEK_SET)
    g->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    g->l += offset;

  // for a growing narrator, the "end" is the greatest position ever written to
  else if(whence == NARRATOR_SEEK_END)
    g->l = (g->m + offset);

  return g->l;
}

API off_t narrator_growing_reset(narrator * restrict n)
{
  return narrator_growing_seek(n, 0, NARRATOR_SEEK_SET);
}

API size_t narrator_growing_read(narrator * restrict n, void * dst, size_t count)
{
  narrator_growing * g = &n->growing;
  size_t d = MIN(count, g->m - g->l);
  memcpy(dst, g->s + g->l, d);
  g->l += d;
  return d;
}
