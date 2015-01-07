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

#ifndef _LISTWISE_DESCRIBE_INTERNAL_H
#define _LISTWISE_DESCRIBE_INTERNAL_H

#define restrict __restrict

typedef int (*fwriter)(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...);

/// writes to a fixed size buffer
int zwrite(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 3, 5)));

/// writes to a dynamically-resizing pstring
int pswrite(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 4, 5)));

#define SAY(fmt, ...) fatal(writer, dst, sz, z, ps, fmt, ##__VA_ARGS__)

#undef restrict
#endif
