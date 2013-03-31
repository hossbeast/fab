/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "pstring.h"

#include "xmem.h"

int pvprintf(pstring ** p, char * fmt, va_list va)
{
	if(!*p && xmalloc(p, sizeof(**p)) == 0)
		return 0;

	va_list va2;
	va_copy(va2, va);

	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if(psgrow(p, size) == 0)
		return 0;

	vsprintf((*p)->s, fmt, va);
	(*p)->l = size;

	return 1;
}

int psprintf(pstring ** p, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	if(pvprintf(p, fmt, va) == 0)
		return 0;
	va_end(va);

	return 1;
}

int pscatvf(pstring ** p, char * fmt, va_list va)
{
	if(!*p && xmalloc(p, sizeof(**p)) == 0)
		return 0;

	va_list va2;
	va_copy(va2, va);

	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if(psgrow(p, (*p)->l + size) == 0)
		return 0;

	vsprintf((*p)->s + (*p)->l, fmt, va);
	(*p)->l += size;

	return 1;
}

int pscatf(pstring ** p, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	if(pscatvf(p, fmt, va) == 0)
		return 0;

	va_end(va);

	return 1;
}

int pscat(pstring ** p, void * s, size_t l)
{
	if(!*p && xmalloc(p, sizeof(**p)) == 0)
		return 0;

	if(psgrow(p, (*p)->l + l) == 0)
		return 0;

	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->l += l;

	return 1;
}

int psgrow(pstring ** p, size_t l)
{
	if(!*p && xmalloc(p, sizeof(**p)) == 0)
		return 0;

	if(l >= (*p)->a)
	{
		if(xrealloc(&(*p)->s, 1, l+1, (*p)->a) == 0)
			return 0;

		(*p)->a = l+1;
	}

	return 1;
}

void pstring_free(pstring * p)
{
	if(p)
		free(p->s);

	free(p);
}

void pstring_xfree(pstring ** p)
{
	pstring_free(*p);
	*p = 0;
}

