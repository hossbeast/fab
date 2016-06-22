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

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "fixed.internal.h"

#include "macros.h"

#define restrict __restrict

//
// public
//

xapi fixed_vsayf(narrator_fixed * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  n->l += znvloadf(n->s + n->l, n->a - n->l, fmt, va);

  finally : coda;
}

xapi fixed_sayw(narrator_fixed * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  n->l += znloadw(n->s + n->l, n->a - n->l, b, l);

  finally : coda;
}

xapi fixed_seek(narrator_fixed * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  enter;

  if(whence == NARRATOR_SEEK_SET)
    n->l = offset;
  else if(whence == NARRATOR_SEEK_CUR)
    n->l += offset;

  // for a fixed narrator, the "end" is the allocated size
  else if(whence == NARRATOR_SEEK_END)
    n->l = (n->a + offset);

  if(res)
    *res = n->l;

  finally : coda;
}

void fixed_destroy(narrator_fixed * n)
{
  free(n->s);
}

//
// api
//

API xapi narrator_fixed_create(narrator ** const restrict rv, size_t size)
{
  enter;

  narrator * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  n->type = NARRATOR_FIXED;
  n->fixed.a = size + 1;    // trailing null byte

  fatal(xmalloc, &n->fixed.s, sizeof(*n->fixed.s) * n->fixed.a);

  *rv = n;
  n = 0;

finally:
  narrator_free(n);
coda;
}

API const char * narrator_fixed_buffer(narrator * const restrict n)
{
  return n->fixed.s;
}

API size_t narrator_fixed_size(narrator * const restrict n)
{
  return n->fixed.l;
}
