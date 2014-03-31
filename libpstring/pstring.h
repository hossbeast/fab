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

#ifndef _PSTRING_H
#define _PSTRING_H

#include <stdarg.h>
#include <sys/types.h>

#define restrict __restrict

///
/// pstring - string storage plus allocation and length for reuse
///  invariant : s + l == 0
///
typedef struct pstring
{
	size_t		a;
	size_t		l;
	char			s[];
} pstring;

/// psvprintf
//
// SUMMARY
//  vsprintf for pstring - set its contents
//
// PARAMETERS
//  p - pstring
//
int psvprintf(pstring ** restrict p, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

/// psprintf
//
// SUMMARY
//  sprintf for pstring - set its contents
//
// PARAMETERS
//  p - pstring
//
int psprintf(pstring ** restrict p, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

/// psprints
//
// SUMMARY
//  copy { s, s + strlen(s) } to the contents of the pstring
//
// PARAMETERS
//  p - pstring
//
int psprints(pstring ** restrict p, char * const restrict s)
	__attribute__((nonnull));

/// psprint
//
// SUMMARY
//  copy { s, s + l } to the contents of the pstring
//
// PARAMETERS
//  p - pstring
//  s - source bytes
//  l - length of s (0 : no-op)
//
int psprint(pstring ** restrict p, char * const restrict s, size_t l)
	__attribute__((nonnull));

/// psvcatf
//
// SUMMARY
//  vsprintf for pstring - concatenate to its contents
//
// PARAMETERS
//  p - pstring
//
int psvcatf(pstring ** restrict p, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

/// pscatf
//
// SUMMARY
//  sprintf for pstring - concatenate to its contents
//
// PARAMETERS
//  p - pstring
//
int pscatf(pstring ** restrict p, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

/// pscats
//
// SUMMARY
//  concatenate { s, s + strlen(s) } to the contents of the pstring
//
// PARAMETERS
//  p - pstring
//
int pscats(pstring ** restrict p, char * const restrict s)
	__attribute__((nonnull));

/// pscat
//
// SUMMARY
//  concatenate { s, s + l } to the contents of the pstring
//
// PARAMETERS
//  p - pstring
//  s - source bytes
//  l - length of s (0 : no-op)
//
int pscat(pstring ** restrict p, char * const restrict s, size_t l)
	__attribute__((nonnull));

/// psgrow
//
// SUMMARY
//  grow the allocation of the pstring arbitrarily
//
// PARAMETERS
//  p - pstring
//
int psgrow(pstring ** restrict p, size_t l)
	__attribute__((nonnull));

/// psclear
//
// SUMMARY
//  set the internal length of the pstring to zero ; the allocation remains intact
//
// PARAMETERS
//  p - pstring
//
int psclear(pstring ** restrict p)
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

/// pswfree
//
// SUMMARY
//  free a pstring with wfree semantics
//
// PARAMETERS
//  p - pstring
//
void pswfree(pstring **)
	__attribute__((nonnull));

#undef restrict
#endif
