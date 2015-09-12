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

API xapi psgrow(pstring ** p, size_t l)
{
  enter;

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

API xapi psclear(pstring ** restrict p)
{
  enter;

	fatal(psgrow, p, 64);

	(*p)->l = 0;

	finally : coda;
}

API void psfree(pstring * p)
{
	free(p);
}

API void pswfree(pstring ** p)
{
	psfree(*p);
	*p = 0;
}

// load

API xapi psvloadf(pstring ** restrict p, const char * const restrict fmt, va_list va)
{
  enter;

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

API xapi psloadf(pstring ** restrict p, const char * const restrict fmt, ...)
{
  enter;

	va_list va;
	va_start(va, fmt);
	fatal(psvloadf, p, fmt, va);
	va_end(va);

	finally : coda;
}

API xapi psloads(pstring ** restrict p, char * const restrict s)
{
	xproxy(psloadw, p, s, strlen(s));
}

API xapi psloadw(pstring ** restrict p, char * const restrict s, size_t l)
{
  enter;

	fatal(psgrow, p, 64);
	fatal(psgrow, p, l);
	memcpy((*p)->s, s, l);
	(*p)->s[l] = 0;
	(*p)->l = l;

	finally : coda;
}

API xapi psloadc(pstring ** restrict p, int c)
{
  enter;

	fatal(psgrow, p, 64);
	(*p)->s[0] = c;
	(*p)->s[1] = 0;
	(*p)->l = 1;

	finally : coda;
}

// cat

API xapi psvcatf(pstring ** restrict p, const char * const restrict fmt, va_list va)
{
  enter;

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

API xapi pscatf(pstring ** restrict p, const char * const restrict fmt, ...)
{
  enter;

	va_list va;
	va_start(va, fmt);
	fatal(psvcatf, p, fmt, va);
	va_end(va);

	finally : coda;
}

API xapi pscats(pstring ** restrict p, char * const restrict s)
{
	xproxy(pscatw, p, s, strlen(s));
}

API xapi pscatw(pstring ** restrict p, char * const restrict s, size_t l)
{
  enter;

	fatal(psgrow, p, 64);
	fatal(psgrow, p, (*p)->l + l + 1);
	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->s[(*p)->l + l] = 0;
	(*p)->l += l;

	finally : coda;
}

API xapi pscatc(pstring ** restrict p, int c)
{
  enter;

	fatal(psgrow, p, 64);
	fatal(psgrow, p, (*p)->l + 1 + 1);
	(*p)->s[(*p)->l] = c;
	(*p)->s[(*p)->l + 1] = 0;
	(*p)->l++;

	finally : coda;
}

// mk

API xapi psvmkf(pstring * restrict e, pstring ** restrict p, const char * const restrict fmt, va_list va)
{
  enter;

	pstring * lp;
	if(!p)
		p = &lp;
	*p = e;

	fatal(psvcatf, p, fmt, va);

	finally : coda;
}

API xapi psmkf(pstring * restrict e, pstring ** restrict p, const char * const restrict fmt, ...)
{
  enter;

	va_list va;
	va_start(va, fmt);
	fatal(psvmkf, e, p, fmt, va);
	va_end(va);

	finally : coda;
}

API xapi psmks(pstring * restrict e, char * const restrict s, pstring ** restrict p)
{
	xproxy(psmkw, e, s, strlen(s), p);
}

API xapi psmkw(pstring * restrict e, char * const restrict s, size_t l, pstring ** restrict p)
{
  enter;

	pstring * lp;
	if(!p)
		p = &lp;
	*p = e;

	fatal(pscatw, p, s, l);

	finally : coda;
}

API xapi psmkc(pstring * restrict e, int c, pstring ** restrict p)
{
  enter;

	pstring * lp;
	if(!p)
		p = &lp;
	*p = e;

	fatal(pscatc, p, c);

	finally : coda;
}
