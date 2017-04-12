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

#ifndef _XAPI_INFO_H
#define _XAPI_INFO_H

#if XAPI_STACKTRACE

#include <stdarg.h>
#include <sys/types.h>

#define restrict __restrict

/// xapi_info_unstage
//
// SUMMARY
//  unstage staged infos, if any
//
void xapi_info_unstage(void);

/// xapi_info_
//
// SUMMARY
//  add an info key/value pair to the staging area. the info is silently discarded if the key is
//  null or has zero length.
//
// VARIANTS
//  push    - append
//  unshift - prepend
//

void xapi_info_pushs(const char * restrict key, const char * restrict vstr);
void xapi_info_pushw(const char * restrict key, const char * restrict vbuf, size_t vlen);
void xapi_info_pushf(const char * restrict key, const char * restrict vfmt, ...)
  __attribute__((format(printf, 2, 3)));
void xapi_info_pushvf(const char * restrict key, const char * restrict vfmt, va_list va);

void xapi_info_unshifts(const char * restrict key, const char * restrict vstr);
void xapi_info_unshiftw(const char * restrict key, const char * restrict vbuf, size_t vlen);
void xapi_info_unshiftf(const char * restrict key, const char * restrict vfmt, ...)
  __attribute__((format(printf, 2, 3)));
void xapi_info_unshiftvf(const char * restrict key, const char * restrict vfmt, va_list va);

void xapi_info_inserts(uint16_t index, const char * restrict key, const char * restrict vstr);
void xapi_info_insertw(uint16_t index, const char * restrict key, const char * restrict vbuf, size_t vlen);
void xapi_info_insertf(uint16_t index, const char * restrict key, const char * restrict vfmt, ...)
  __attribute__((format(printf, 3, 4)));
void xapi_info_insertvf(uint16_t index, const char * restrict key, const char * restrict vfmt, va_list va);

#undef restrict
#endif
#endif
