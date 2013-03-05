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
