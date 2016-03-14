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

#include "xapi.h"
#include "xlinux.h"

#include "assure.h"

#define DEFAULT_SEED 10
#define restrict __restrict

xapi assure(void * target, size_t es, size_t len, size_t * const restrict ac)
{
  xproxy(assurex, target, es, len, ac, DEFAULT_SEED);
}

xapi assurex(void * target, size_t es, size_t len, size_t * const restrict ac, size_t seed)
{
  enter;

  void ** p = (void**)target;

	if(!*p || len >= *ac)
	{
		size_t nc = *ac ?: seed;

		while(nc <= *ac)
			nc = nc * 2 + nc / 2;

		fatal(xrealloc, p, es, nc, *ac);
    *ac = nc;
	}

	finally : coda;
}
