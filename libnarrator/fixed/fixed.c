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

#include "xlinux/xstdlib.h"

#include "internal.h"
#include "fixed.internal.h"

#include "zbuffer.h"

#define restrict __restrict

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

API void narrator_fixed_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  narrator_fixed * f = &n->fixed;

  f->l += znloadvf(f->s + f->l, f->a - f->l, fmt, va);
}

API void narrator_fixed_sayw(narrator * const restrict n, const char * const restrict b, size_t l)
{
  narrator_fixed * f = &n->fixed;

  f->l += znloadw(f->s + f->l, f->a - f->l, b, l);
}

API off_t narrator_fixed_seek(narrator * const restrict n, off_t offset, int whence)
{
  narrator_fixed * f = &n->fixed;

  if(whence == NARRATOR_SEEK_SET)
    f->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    f->l += offset;

  // for a fixed narrator, the "end" is the allocated size
  else if(whence == NARRATOR_SEEK_END)
    f->l = (f->a + offset);

  return f->l;
}

API off_t narrator_fixed_reset(narrator * const restrict n)
{
  return narrator_fixed_seek(n, 0, SEEK_SET);
}

API size_t narrator_fixed_read(narrator * restrict n, void * dst, size_t count)
{
  narrator_fixed * f = &n->fixed;
  size_t d = MIN(count, f->a - f->l);
  memcpy(dst, f->s + f->l, d);
  f->l += d;
  return d;
}
