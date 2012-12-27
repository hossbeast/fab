#ifndef _VSTRING_H
#define _VSTRING_H

#include <stdarg.h>
#include <sys/types.h>

#define restrict __restrict

typedef struct
{
	char *		s;
	size_t		a;
	size_t		l;
} pstring;

int pvprintf(pstring ** restrict p, char * restrict fmt, va_list va)
	__attribute__((nonnull));

int psprintf(pstring ** restrict p, char * restrict fmt, ...)
	__attribute__((nonnull));

int pscatvf(pstring ** restrict p, char * restrict fmt, va_list va)
	__attribute__((nonnull));

int pscatf(pstring ** restrict p, char * restrict fmt, ...)
	__attribute__((nonnull));

int pscat(pstring ** restrict p, void * restrict s, size_t l)
	__attribute__((nonnull));

int psgrow(pstring ** restrict p, size_t l)
	__attribute__((nonnull));

int pstring_free(pstring *);

int pstring_xfree(pstring **)
	__attribute__((nonnull));

#undef restrict
#endif
