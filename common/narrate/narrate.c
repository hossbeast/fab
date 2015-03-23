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

#include "pstring.h"

#include "say.h"

int render(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
{
	if(sz - (*z))
	{
		va_list va;
		va_start(va, fmt);
		(*z) += MIN(sz, vsnprintf(dst + (*z), sz - (*z), fmt, va));
		va_end(va);
	}

	finally : coda;
}

int psrender(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
{
	size_t ol = 0;
	if(*ps)
		ol = (*ps)->l;

	va_list va;
	va_start(va, fmt);
	fatal(psvcatf, ps, fmt, va);
	va_end(va);

	(*z) += (*ps)->l - ol;

	finally : coda;
}
