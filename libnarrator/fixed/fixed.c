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
#include <stdarg.h>
#include <stdlib.h>

#include "types.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "fixed.internal.h"

#include "zbuffer.h"

//
// public
//

int fixed_sayvf(narrator_fixed * const restrict n, const char * const restrict fmt, va_list va)
{
  size_t l = znloadvf(n->s + n->l, n->a - n->l, fmt, va);
  n->l += l;
  n->m = MAX(n->l, n->m);
  return l;
}

int fixed_sayw(narrator_fixed * const restrict n, const char * const restrict b, size_t l)
{
  n->l += znloadw(n->s + n->l, n->a - n->l, b, l);
  n->m = MAX(n->l, n->m);
  return (int)l;
}

off_t fixed_seek(narrator_fixed * const restrict n, off_t offset, int whence)
{
  if(whence == NARRATOR_SEEK_SET)
    n->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    n->l += offset;

  // for a fixed narrator, the "end" is the allocated size
  else if(whence == NARRATOR_SEEK_END)
    n->l = (n->a + offset);

  return n->l;
}

off_t fixed_reset(narrator_fixed * const restrict n)
{
  return fixed_seek(n, 0, SEEK_SET);
}

size_t fixed_read(narrator_fixed * restrict n, void * dst, size_t count)
{
  size_t d = MIN(count, n->m - n->l);
  memcpy(dst, n->s + n->l, d);
  n->l += d;
  return d;
}

//
// api
//

API narrator * narrator_fixed_init(narrator_fixed_storage * restrict fixed)
{
  narrator * n = (void*)fixed;
  n->type = NARRATOR_FIXED;
  n->fixed.l = 0;
  return n;
}

API const char * narrator_fixed_buffer(narrator * const restrict n)
{
  return n->fixed.s;
}

API size_t narrator_fixed_size(narrator * const restrict n)
{
  return n->fixed.l;
}
