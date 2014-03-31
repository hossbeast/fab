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
#include "xlinux/xstdlib.h"

#include "internal.h"

#define restrict __restrict

///
/// public
///

int API psgrow(pstring ** p, size_t l)
{
	if(!*p || l >= (*p)->a)
	{
		size_t oec = 0;
		int ns = 6;

		if(*p)
		{
			ns = (*p)->a;
			oec = sizeof(**p) + (*p)->a;
		}

		while(ns <= l)
			ns = ns * 2 + ns / 2;

		fatal(xrealloc, p, 1, sizeof(**p) + ns, oec);
		(*p)->a = ns;
	}

	finally : coda;
}

int API psclear(pstring ** restrict p)
{
	fatal(psgrow, p, 100);

	(*p)->l = 0;

	finally : coda;
}

int API psvprintf(pstring ** restrict p, const char * const restrict fmt, va_list va)
{
	fatal(psgrow, p, 100);

	va_list va2;
	va_copy(va2, va);
	int r = vsnprintf((*p)->s, (*p)->a, fmt, va2);
	va_end(va2);

	if(r >= (*p)->a)
	{
		fatal(psgrow, p, r);
		vsprintf((*p)->s, fmt, va);
	}

	(*p)->l = r;

	finally : coda;
}

int API psprintf(pstring ** restrict p, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(psvprintf, p, fmt, va);
	va_end(va);

	finally : coda;
}

int API psprints(pstring ** restrict p, char * const restrict s)
{
	xproxy(psprint, p, s, strlen(s));
}

int API psprint(pstring ** restrict p, char * const restrict s, size_t l)
{
	fatal(psgrow, p, 100);
	fatal(psgrow, p, l);
	memcpy((*p)->s, s, l);
	(*p)->s[(*p)->l] = 0;
	(*p)->l = l;

	finally : coda;
}

int API psvcatf(pstring ** restrict p, const char * const restrict fmt, va_list va)
{
	fatal(psgrow, p, 100);

	va_list va2;
	va_copy(va2, va);
	int r = vsnprintf((*p)->s + (*p)->l, (*p)->a - (*p)->l, fmt, va2);
	va_end(va2);

	if(r >= ((*p)->a - (*p)->l))
	{
		fatal(psgrow, p, (*p)->l + r);
		vsprintf((*p)->s + (*p)->l, fmt, va);
	}
	(*p)->l += r;

	finally : coda;
}

int API pscatf(pstring ** restrict p, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(psvcatf, p, fmt, va);
	va_end(va);

	finally : coda;
}

int API pscats(pstring ** restrict p, char * const restrict s)
{
	xproxy(pscat, p, s, strlen(s));
}

int API pscat(pstring ** restrict p, char * const restrict s, size_t l)
{
	fatal(psgrow, p, 100);
	fatal(psgrow, p, (*p)->l + l);
	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->l += l;

	finally : coda;
}

void API psfree(pstring * p)
{
	free(p);
}

void API pswfree(pstring ** p)
{
	psfree(*p);
	*p = 0;
}
