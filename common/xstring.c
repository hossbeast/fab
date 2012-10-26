#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "xstring.h"

#include "xmem.h"

int xstrcmp(char* A, int alen, char* B, int blen, int case_insensitive)
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

char* xstrstr(char* A, int alen, char* B, int blen, int case_insensitive)
{
	int x, y;
	for(x = 0; x < (alen - blen); x++)
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

int xstrcat(char** s1, const char* s2)
{
	char* o = *s1;
	int s1len = 0;
	if(o)
		s1len = strlen(o);

	int s2len = strlen(s2);

	if(xmalloc(s1, s1len + s2len + 1))
	{
		if(o)
			memcpy(*s1, o, s1len);

		free(o);
		memcpy((*s1) + s1len, s2, s2len);
	}
	else
		return 0;

	return 1;
}

int xstrcatf(char** s, char* fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	int req = vsnprintf(0, 0, fmt, va);
	va_end(va);

	char* o = *s;
	int len = 0;
	if(o)
		len = strlen(o);
	if(xmalloc(s, len + req + 1))
	{
		if(o)
			memcpy(*s, o, len);

		free(o);
		va_start(va, fmt);
		vsprintf((*s) + len, fmt, va);
		va_end(va);
	}
	else
		return 0;

	return 1;
}

int xstrdup(char** s1, const char* s2)
{
	int s1len = 0;
	int s2len = strlen(s2);
	if(*s1)
		s1len = strlen(*s1);

	if(*s1 && s1len >= s2len)
	{
		memcpy(*s1, s2, s2len + 1);
		return 1;
	}

	free(*s1);
	*s1 = 0;

	return xstrcat(s1, s2);
}

int xsprintf(char** s, char* fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	int req = vsnprintf(0, 0, fmt, va);
	va_end(va);

	free(*s);
	if(xmalloc(s, req + 1))
	{
		va_start(va, fmt);
		vsprintf((*s), fmt, va);
		va_end(va);
	}
	else
		return 0;

	return 1;
}
