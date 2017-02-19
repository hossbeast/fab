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

#ifndef _VALYRIA_STRUTIL_H
#define _VALYRIA_STRUTIL_H

/*

SUMMARY
 dynamically resizing string

*/

#include <stdarg.h>
#include <sys/types.h>

#include "xapi.h"

xapi strloadc(char ** restrict dst, int c)
  __attribute__((nonnull));

xapi strloads(char ** restrict dst, const char * restrict s)
  __attribute__((nonnull));

xapi strloadw(char ** restrict dst, const void * restrict buf, size_t bufl)
  __attribute__((nonnull));

xapi strloadf(char ** restrict dst, const char * restrict fmt, ...)
  __attribute__((nonnull));

xapi strloadvf(char ** restrict dst, const char * restrict fmt, va_list va)
  __attribute__((nonnull));

#undef restrict
#endif
