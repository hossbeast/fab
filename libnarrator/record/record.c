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
#include "xlinux.h"

#include "internal.h"
#include "record.internal.h"

#include "macros.h"
#include "assure.h"

#define restrict __restrict

//
// public
//

xapi record_vsayf(narrator_record * const restrict n, const char * const restrict fmt, va_list va)
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

  finally : coda;
}

xapi record_sayw(narrator_record * const restrict n, const char * const restrict b, size_t l)
{
  enter;

  fatal(assure, &n->s, sizeof(*n->s), n->l + l + 1, &n->a);
  memcpy(n->s + n->l, b, l);
  n->s[n->l + l] = 0;
  n->l += l;

  finally : coda;
}

xapi record_seek(narrator_record * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  xproxy(narrator_seek, n->n, offset, whence, res);
}

void record_destroy(narrator_record * n)
{
  free(n->s);
}

//
// api
//

API xapi narrator_record_create(narrator ** const restrict rv, narrator * const restrict np)
{
  enter;

  narrator * n = 0;
  fatal(xmalloc, &n, sizeof(*n));

  n->type = NARRATOR_RECORD;
  n->record.n = np;
  
  *rv = n;
  n = 0;

finally:
  narrator_free(n);
coda;
}

API xapi narrator_record_write(narrator * const restrict n)
{
  enter;

  // flush to the underlying narrator and reset
  fatal(narrator_sayw, n->record.n, n->record.s, n->record.l);
  n->record.l = 0; 

  finally : coda;
}