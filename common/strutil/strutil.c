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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "strutil.h"

#include "wstdlib.h"

int estrcmp(char* A, int alen, char* B, int blen, int case_insensitive)
{
	if(!alen)
		alen = strlen(A);
	if(!blen)
		blen = strlen(B);

	int x, result = 0;
	for(x = 0; x < alen && x < blen && result == 0; x++)
	{
		int adelta = 0;
		int bdelta = 0;
		if(case_insensitive)
		{
			if(A[x] >= 'A' && A[x] <= 'Z')
				adelta = ('a' - 'A');
			if(B[x] >= 'A' && B[x] <= 'Z')
				bdelta = ('a' - 'A');
		}

		result = (((unsigned char)A[x]) + adelta) - (((unsigned char)B[x]) + bdelta);
	}
	if(result == 0 && alen != blen)
	{
		if(alen > blen)
			return 1;
		else
			return -1;
	}
	return result;
}

char* estrstr(char * A, int alen, const char * const B, int blen, const int case_insensitive)
{
	alen = alen ?: strlen(A);
	blen = blen ?: strlen(B);

	if(blen == 0)
		return 0;

	int x, y;
	for(x = 0; x <= (alen - blen); x++)
	{
		for(y = 0; y < blen; y++)
		{
			int adelta = 0;
			int bdelta = 0;
			if(case_insensitive)
			{
				if(A[x + y] >= 'A' && A[x + y] <= 'Z')
					adelta = ('a' - 'A');
				if(B[y] >= 'A' && B[y] <= 'Z')
					bdelta = ('a' - 'A');
			}
			if(((((unsigned char)A[x + y]) + adelta) - (((unsigned char)B[y]) + bdelta)) != 0)
				break;
		}
		if(y == blen)
			return A + x;
	}
	return 0;
}

int stresc(char * const src, const size_t len, char * const dst, const size_t sz)
{
	size_t z  = 0;
	int x;
	for(x = 0; x < (len ?: strlen(src)); x++)
	{
		if(src[x] == 0x09)
			z += snprintf(dst + z, sz - z, "\\t");
		else if(src[x] == 0x0a)
			z += snprintf(dst + z, sz - z, "\\n");
		else if(src[x] == 0x0d)
			z += snprintf(dst + z, sz - z, "\\r");
		else if(src[x] >= 0x20 && src[x] <= 0x7e)
			z += snprintf(dst + z, sz - z, "%c", src[x]);
		else
			z += snprintf(dst + z, sz - z, "\\x%02hhx", src[x]);
	}

	return z;
}
