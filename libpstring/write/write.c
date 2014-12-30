/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "internal.h"

#define restrict __restrict

//
// api
//

int API pdwritevf(int fd, pstring ** restrict p, const char * const restrict fmt, va_list va)
{
	fatal(psvwritef, p, fmt, va);

	write(fd, (*p)->s, (*p)->l);

	finally : coda;
}

int API pdwritef(int fd, pstring ** restrict p, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(pdwritevf, fd, p, fmt, va);
	va_end(va);

	finally : coda;
}

int API pdwrites(int fd, pstring ** restrict p, char * const restrict s)
{
	xproxy(pdwritew, p, s, strlen(s));
}

int API pdwritew(int fd, pstring ** restrict p, char * const restrict s, size_t l)
{
	fatal(pswritew, p, s, l);

	

	fatal(psgrow, p, 100);
	fatal(psgrow, p, (*p)->l + l);
	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->l += l;

	finally : coda;
}
