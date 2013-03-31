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

void pstring_free(pstring *);

void pstring_xfree(pstring **)
	__attribute__((nonnull));

#undef restrict
#endif

