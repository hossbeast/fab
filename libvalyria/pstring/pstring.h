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

#ifndef _VALYRIA_PSTRING_H
#define _VALYRIA_PSTRING_H

/*

SUMMARY
 dynamically resizing string

*/

#include <stdarg.h>
#include <sys/types.h>

#include "xapi.h"

typedef struct pstring
{
  char * s;   // buffer
  size_t l;   // size

#ifndef PSTRING_INTERNALS
# define PSTRING_INTERNALS
#endif
  PSTRING_INTERNALS;
} pstring;

#define restrict __restrict

/// pscreate
//
// SUMMARY
//  create an empty pstring
//
// PARAMETERS
//  ps         - (returns) pstring
//  [capacity] - initial capacity
//
xapi pscreate(pstring ** const restrict ps)
  __attribute__((nonnull));

xapi pscreatex(pstring ** const restrict ps, size_t capacity)
  __attribute__((nonnull));

/// psmkw
//
// SUMMARY
//  creates a new pstring or reuses an existing instance, and appends like pscat
//
// PARAMETERS
//  ps    - (returns) pstring
//  [e]   - existing instance
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [s]   - null-terminated string
//  [b]   - buffer
//  [l]   - buffer length
//  [c]   - single byte
//
xapi psvmkf(pstring ** restrict ps, pstring * restrict e, const char * const restrict fmt, va_list va)
  __attribute__((nonnull(1,3)));

xapi psmkf(pstring ** restrict ps, pstring * restrict e, const char * const restrict fmt, ...)
  __attribute__((nonnull(1,3)));

xapi psmks(pstring ** restrict ps, pstring * restrict e, const char * const restrict s)
  __attribute__((nonnull(1,3)));

xapi psmkw(pstring ** restrict ps, pstring * restrict e, const char * const restrict b, size_t l)
  __attribute__((nonnull(1,3)));

xapi psmkc(pstring ** restrict ps, pstring * restrict e, int c)
  __attribute__((nonnull(1)));

/// psfree
//
// SUMMARY
//  free a pstring with free semantics
//
// PARAMETERS
//  p - pstring
//
void psfree(pstring * restrict ps);

/// psifree
//
// SUMMARY
//  free a pstring with iwfree semantics
//
// PARAMETERS
//  p - pstring
//
void psifree(pstring ** const restrict ps)
  __attribute__((nonnull));

/// psclear
//
// SUMMARY
//  set the internal length of the pstring to zero; the allocation remains intact
//
// PARAMETERS
//  p - pstring
//
void psclear(pstring * restrict p)
  __attribute__((nonnull));

/// psload
//
// SUMMARY
//  set the value of a pstring
//
// PARAMETERS
//  p     - pass/return pstring instance
//  [fmt] - printf-style format string
//  [va]  - va_args
//  [s]   - null-terminated string
//  [b]   - buffer
//  [l]   - buffer length
//  [c]   - single byte
//
// VARIANTS
//  psloadvf : load formatted output from va_list
//  psloadf : load formatted output from varargs
//  psloads : load a string
//  psloadw : load a buffer
//  psloadc : load a byte
//

xapi psloadvf(pstring * restrict p, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi psloadf(pstring * restrict p, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi psloads(pstring * restrict p, const char * const restrict s)
  __attribute__((nonnull));

xapi psloadw(pstring * restrict p, const char * const restrict b, size_t l)
  __attribute__((nonnull));

xapi psloadc(pstring * restrict p, int c)
  __attribute__((nonnull));

/// pscat
//
// SUMMARY
//  see psload, except pscat appends instead of overwriting
//
// PARAMETERS
//  [fmt] - printf-style format string
//  [va]  - va_args
//  [s]   - null-terminated string
//  [b]   - buffer
//  [l]   - buffer length
//  [c]   - single byte
//
xapi psvcatf(pstring * restrict p, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi pscatf(pstring * restrict p, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi pscats(pstring * restrict p, const char * const restrict s)
  __attribute__((nonnull));

xapi pscatw(pstring * restrict p, const char * const restrict b, size_t l)
  __attribute__((nonnull));

xapi pscatc(pstring * restrict p, int c)
  __attribute__((nonnull));

/// pscmp
//
// SUMMARY
//  compare two pstring instances
//
// PARAMETERS
//  A - pstring
//  B - pstring
//
int pscmp(const pstring * const restrict A, const pstring * const restrict B)
  __attribute__((nonnull));

#undef restrict
#endif
