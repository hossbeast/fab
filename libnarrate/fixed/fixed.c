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

#include "internal.h"
#include "fixed.h"
#include "fixed.internal.h"

#include "macros.h"

#define restrict __restrict

//
// public
//

xapi fixed_vsayf(char * const restrict dst, size_t sz, size_t * const restrict szo, const char * const restrict fmt, va_list va)
{
  enter;

  *szo += znvloadf(dst + *szo, sz - *szo, fmt, va);

  finally : coda;
}

xapi fixed_sayw(char * const restrict dst, size_t sz, size_t * const restrict szo, char * const restrict b, size_t l)
{
  enter;

  *szo += znloadw(dst + *szo, sz - *szo, b, l);

  finally : coda;
}

//
// api
//

void API narrate_fsayf(narrator * n, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	narrate_fvsayf(n, fmt, va);

	va_end(va);
}

void API narrate_fsayc(narrator * const restrict n, int c)
{
	narrate_fsayw(n, (char*)&c, 1);
}

void API narrate_fvsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  n->bz += znvloadf(n->bb + n->bz, n->bsz - n->bz, fmt, va);
}

void API narrate_fsayw(narrator * const restrict n, char * const restrict b, size_t l)
{
  n->bz += znloadw(n->bb + n->bz, n->bsz - n->bz, b, l);
}
