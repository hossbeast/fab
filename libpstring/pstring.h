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

#ifndef _PSTRING_H
#define _PSTRING_H

#include <stdarg.h>
#include <sys/types.h>

#include "xapi.h"

#define restrict __restrict

/// pstring
//
// SUMMARY
//  string storage plus allocation and length for reuse
//  invariant : *(s + l) == 0
//
typedef struct pstring
{
	size_t		a;
	size_t		l;
	char			s[];
} pstring;

/// psgrow
//
// SUMMARY
//  grow the allocation of the pstring arbitrarily
//
// PARAMETERS
//  p - pstring
//
xapi psgrow(pstring ** restrict p, size_t l)
	__attribute__((nonnull));

/// psclear
//
// SUMMARY
//  set the internal length of the pstring to zero ; the allocation remains intact
//
// PARAMETERS
//  p - pstring
//
xapi psclear(pstring ** restrict p)
	__attribute__((nonnull));

/// psfree
//
// SUMMARY
//  free a pstring with free semantics
//
// PARAMETERS
//  p - pstring
//
void psfree(pstring *);

/// psxfree
//
// SUMMARY
//  free a pstring with xfree semantics
//
// PARAMETERS
//  p - pstring
//
void psxfree(pstring **)
	__attribute__((nonnull));

/// psload
//
// SUMMARY
//  set the value of a pstring
//
// PARAMETERS
//  p   - pass/return pstring instance
//  fmt - printf-style format string
//  va  - va_args
//  s   - pointer to string
//  l   - length of string (0 : no-op)
//  c   - single byte
//
// VARIANTS
//  psvloadf : load formatted output from va_list
//  psloadf : load formatted output from varargs
//  psloads : load a string
//  psloadw : load a buffer
//  psloadc : load a byte
//

xapi psvloadf(pstring ** restrict p, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

xapi psloadf(pstring ** restrict p, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

xapi psloads(pstring ** restrict p, const char * const restrict s)
	__attribute__((nonnull));

xapi psloadw(pstring ** restrict p, const char * const restrict s, size_t l)
	__attribute__((nonnull));

xapi psloadc(pstring ** restrict p, int c)
	__attribute__((nonnull));

/// pscat
//
// SUMMARY
//  see psload, except pscat appends instead of overwriting
//
xapi psvcatf(pstring ** restrict p, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

xapi pscatf(pstring ** restrict p, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

xapi pscats(pstring ** restrict p, const char * const restrict s)
	__attribute__((nonnull));

xapi pscatw(pstring ** restrict p, const char * const restrict s, size_t l)
	__attribute__((nonnull));

xapi pscatc(pstring ** restrict p, int c)
	__attribute__((nonnull));

/// psmkw
//
// SUMMARY
//  see pscat, except psmk accepts an extra parameter, e
//
// PARAMETERS
//  [e] - existing instance
//
xapi psvmkf(pstring * restrict e, pstring ** restrict p, const char * const restrict fmt, va_list va)
	__attribute__((nonnull(2,3)));

xapi psmkf(pstring * restrict e, pstring ** restrict p, const char * const restrict fmt, ...)
	__attribute__((nonnull(2,3)));

xapi psmks(pstring * restrict e, const char * const restrict s, pstring ** restrict p)
	__attribute__((nonnull(2,3)));

xapi psmkw(pstring * restrict e, const char * const restrict s, size_t l, pstring ** restrict p)
	__attribute__((nonnull(2,4)));

xapi psmkc(pstring * restrict e, int c, pstring ** restrict p)
	__attribute__((nonnull(3)));

#undef restrict
#endif
