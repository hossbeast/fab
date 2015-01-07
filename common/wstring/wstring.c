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
#include <stdarg.h>
#include <string.h>

#include "wstdlib.h"

int iwstrcat(char** s1, const char* s2)
{
	char* o = *s1;
	int s1len = 0;
	if(o)
		s1len = strlen(o);

	int s2len = strlen(s2);

	if(wmalloc(s1, s1len + s2len + 1))
		return 1;

	if(o)
		memcpy(*s1, o, s1len);

	free(o);
	memcpy((*s1) + s1len, s2, s2len);

	return 0;
}

int iwstrcatf(char** s, char* fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	int req = vsnprintf(0, 0, fmt, va);
	va_end(va);

	char* o = *s;
	int len = 0;
	if(o)
		len = strlen(o);
	if(wmalloc(s, len + req + 1))
		return 1;
	if(o)
		memcpy(*s, o, len);

	free(o);
	va_start(va, fmt);
	vsprintf((*s) + len, fmt, va);
	va_end(va);

	return 0;
}

int iwstrdup(char** s1, const char* s2)
{
	int s1len = 0;
	int s2len = strlen(s2);
	if(*s1)
		s1len = strlen(*s1);

	if(*s1 && s1len >= s2len)
	{
		memcpy(*s1, s2, s2len + 1);
	}
	else
	{
		free(*s1);
		*s1 = 0;

		return iwstrcat(s1, s2);
	}

	return 0;
}

int iwsprintf(char** s, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int req = vsnprintf(0, 0, fmt, va);
	va_end(va);

	free(*s);
	if(wmalloc(s, req + 1))
		return 1;

	va_start(va, fmt);
	vsprintf((*s), fmt, va);
	va_end(va);

	return 0;
}
