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
#include "xlinux.h"

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

  n->bz += znvloadf(n->bb + n->bz, n->bsz - n->bz, fmt, va);

  finally : coda;
}

xapi fixed_sayw(narrator_fixed * const restrict n, char * const restrict b, size_t l)
{
  enter;

  n->bz += znloadw(n->bb + n->bz, n->bsz - n->bz, b, l);

  finally : coda;
}

xapi fixed_mark(narrator_fixed * const restrict n, size_t * const restrict mark)
{
  enter;

  (*mark) = n->bz;

  finally : coda;
}

void fixed_free(narrator_fixed * n)
{
  if(n)
  {
    free(n->bb);
  }
  free(n);
}

const char * fixed_first(narrator_fixed * const restrict n)
{
  return n->bb;
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
  n->bsz = size + 1;    // trailing null byte

  fatal(xmalloc, &n->fixed.bb, sizeof(*n->fixed.bb) * n->bsz);

  *rv = n;
  n = 0;

finally:
  fixed_free(n);
coda;
}
