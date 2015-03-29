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
	fatal(psgrow, p, 64);

	(*p)->l = 0;

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

// load

int API psvloadf(pstring ** restrict p, const char * const restrict fmt, va_list va)
{
	fatal(psgrow, p, 64);

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

int API psloadf(pstring ** restrict p, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(psvloadf, p, fmt, va);
	va_end(va);

	finally : coda;
}

int API psloads(pstring ** restrict p, char * const restrict s)
{
	xproxy(psloadw, p, s, strlen(s));
}

int API psloadw(pstring ** restrict p, char * const restrict s, size_t l)
{
	fatal(psgrow, p, 64);
	fatal(psgrow, p, l);
	memcpy((*p)->s, s, l);
	(*p)->s[l] = 0;
	(*p)->l = l;

	finally : coda;
}

int API psloadc(pstring ** restrict p, int c)
{
	fatal(psgrow, p, 64);
	(*p)->s[0] = c;
	(*p)->s[1] = 0;
	(*p)->l = 1;

	finally : coda;
}

// cat

int API psvcatf(pstring ** restrict p, const char * const restrict fmt, va_list va)
{
	fatal(psgrow, p, 64);

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
	xproxy(pscatw, p, s, strlen(s));
}

int API pscatw(pstring ** restrict p, char * const restrict s, size_t l)
{
	fatal(psgrow, p, 64);
	fatal(psgrow, p, (*p)->l + l + 1);
	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->s[(*p)->l + l] = 0;
	(*p)->l += l;

	finally : coda;
}

int API pscatc(pstring ** restrict p, int c)
{
	fatal(psgrow, p, 64);
	fatal(psgrow, p, (*p)->l + 1 + 1);
	(*p)->s[(*p)->l] = c;
	(*p)->s[(*p)->l + 1] = 0;
	(*p)->l++;

	finally : coda;
}

// mk

int API psvmkf(pstring * restrict e, pstring ** restrict p, const char * const restrict fmt, va_list va)
{
	pstring * lp;
	if(!p)
		p = &lp;
	*p = e;

	fatal(psvcatf, p, fmt, va);

	finally : coda;
}

int API psmkf(pstring * restrict e, pstring ** restrict p, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fatal(psvmkf, e, p, fmt, va);
	va_end(va);

	finally : coda;
}

int API psmks(pstring * restrict e, char * const restrict s, pstring ** restrict p)
{
	xproxy(psmkw, e, s, strlen(s), p);
}

int API psmkw(pstring * restrict e, char * const restrict s, size_t l, pstring ** restrict p)
{
	pstring * lp;
	if(!p)
		p = &lp;
	*p = e;

	fatal(pscatw, p, s, l);

	finally : coda;
}

int API psmkc(pstring * restrict e, int c, pstring ** restrict p)
{
	pstring * lp;
	if(!p)
		p = &lp;
	*p = e;

	fatal(pscatc, p, c);

	finally : coda;
}
