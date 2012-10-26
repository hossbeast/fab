#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "pstring.h"

int pvprintf(pstring ** p, char * fmt, va_list va)
{
	if(!*p)
		(*p) = calloc(sizeof(**p), 1);

	va_list va2;
	va_copy(va2, va);

	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if(size >= (*p)->a)
	{
		(*p)->a = size + 1;
		(*p)->s = realloc((*p)->s, (*p)->a);
	}

	vsprintf((*p)->s, fmt, va);
	(*p)->l = size;
}

int psprintf(pstring ** p, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	pvprintf(p, fmt, va);
	va_end(va);
}

int pscatvf(pstring ** p, char * fmt, va_list va)
{
	if(!*p)
		*p = calloc(sizeof(**p), 1);

	va_list va2;
	va_copy(va2, va);

	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if(((*p)->l + size) >= (*p)->a)
	{
		(*p)->a = (*p)->l + size + 1;
		(*p)->s = realloc((*p)->s, (*p)->a);
	}

	vsprintf((*p)->s + (*p)->l, fmt, va);
	(*p)->l += size;
}

int pscatf(pstring ** p, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	pscatvf(p, fmt, va);
	va_end(va);
}

int pscat(pstring ** p, void * s, size_t l)
{
	if(!*p)
		*p = calloc(sizeof(**p), 1);

	if(((*p)->l + l) >= (*p)->a)
	{
		(*p)->a = (*p)->l + l + 1;
		(*p)->s = realloc((*p)->s, (*p)->a);
	}

	memcpy((*p)->s + (*p)->l, s, l);
	(*p)->l += l;
}

int psgrow(pstring ** p, size_t l)
{
	if(!*p)
		*p = calloc(sizeof(**p), 1);

	if(l > (*p)->a)
	{
		(*p)->a = l;
		(*p)->s = realloc((*p)->s, (*p)->a);
	}
}

int pstring_free(pstring * p)
{
	if(p)
	{
		free(p->s);
	}
}

int pstring_xfree(pstring ** p)
{
	pstring_free(*p);
	*p = 0;
}
