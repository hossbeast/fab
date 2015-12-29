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
#include <stdlib.h>
#include <string.h>

#include "xapi.h"
#include "xlinux.h"

#include "grow.h"

#define SEED 10
#define restrict __restrict

xapi grow(void * target, size_t es, size_t len, size_t * const restrict ec, size_t * const restrict ac)
{
  xproxy(grow2, target, es, len, ec, ac, SEED);
}

xapi grow2(void * target, size_t es, size_t len, size_t * const restrict ec, size_t * const restrict ac, size_t seed)
{
  enter;

  void ** p = (void**)target;

	if(!*p || (*ec) + len >= *ac)
	{
		int nc = *ac ?: seed;

		while(nc <= *ec)
			nc = nc * 2 + nc / 2;

		fatal(xrealloc, p, es, nc, *ac);
    *ac = nc;
	}

  *ec += len;

	finally : coda;
}
