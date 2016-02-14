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

#include "xapi.h"

#include "internal.h"
#include "fixed.internal.h"

#include "macros.h"

#define restrict __restrict

//
// public
//

xapi fixed_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

  n->bz += znvloadf(n->bb + n->bz, n->bsz - n->bz, fmt, va);

//  *szo += znvloadf(dst + *szo, sz - *szo, fmt, va);

  finally : coda;
}

xapi fixed_sayw(narrator * const restrict n, char * const restrict b, size_t l)
{
  enter;

  n->bz += znloadw(n->bb + n->bz, n->bsz - n->bz, b, l);

//  *szo += znloadw(dst + *szo, sz - *szo, b, l);

  finally : coda;
}

xapi fixed_mark(narrator * const restrict n, size_t * const restrict mark)
{
  enter;

  (*mark) = n->bz;

  finally : coda;
}
