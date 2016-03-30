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
#include "pstring.h"

#include "internal.h"
#include "growing.internal.h"

#define restrict __restrict

//
// public
//

xapi growing_vsayf(narrator_growing * const restrict n, const char * const restrict fmt, va_list va)
{
	xproxy(psvcatf, n->ps, fmt, va);
}

xapi growing_sayw(narrator_growing * const restrict n, char * const restrict b, size_t l)
{
	xproxy(pscatw, n->ps, b, l);
}

xapi growing_mark(narrator_growing * const restrict n, size_t * const restrict mark)
{
  enter;

  (*mark) = n->ps->l;

  finally : coda;
}

void growing_free(narrator_growing * n)
{
  if(n)
  {
    psfree(n->ps);
  }

  free(n);
}

const char * growing_first(narrator_growing * const restrict n)
{
  return n->ps->s;
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
  fatal(pscreate, &n->growing.ps);
  
  *rv = n;
  n = 0;

finally:
  growing_free(n);
coda;
}
