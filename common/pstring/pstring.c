/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#include "pstring.h"

#include "xlinux.h"

int pvprintf(pstring ** p, char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	fatal(psgrow, p, size);

	vsprintf((*p)->s, fmt, va);
	(*p)->l = size;

	finally : coda;
}

int psprintf(pstring ** p, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(pvprintf, p, fmt, va);
	va_end(va);

	finally : coda;
}

int pscatvf(pstring ** p, char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if(!*p)
		fatal(xmalloc, p, sizeof(**p));

	fatal(psgrow, p, (*p)->l + size);

	vsprintf((*p)->s + (*p)->l, fmt, va);
	(*p)->l += size;

	finally : coda;
}

int pscatf(pstring ** p, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(pscatvf, p, fmt, va);
	va_end(va);

	finally : coda;
}

int pscat(pstring ** p, void * s, size_t l)
{
	if(!*p)
		fatal(xmalloc, p, sizeof(**p));

	fatal(psgrow, p, (*p)->l + l);

	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->l += l;

	finally : coda;
}

int psgrow(pstring ** p, size_t l)
{
	if(!*p)
		fatal(xmalloc, p, sizeof(**p));

	if(l >= (*p)->a)
	{
		fatal(xrealloc, &(*p)->s, 1, l+1, (*p)->a);
		(*p)->a = l+1;
	}

	finally : coda;
}

void pstring_free(pstring * p)
{
	if(p)
		free(p->s);

	free(p);
}

void pstring_wfree(pstring ** p)
{
	pstring_free(*p);
	*p = 0;
}
