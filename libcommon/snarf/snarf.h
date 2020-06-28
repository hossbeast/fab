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

#ifndef SNARF_H
#define SNARF_H

#include <stdarg.h>
#include <sys/types.h>

#include "types.h"
#include "xapi.h"

/// snarf
//
// SUMMARY
//  load the contents of a file into a buffer with read
//
// PARAMETERS
//  dst     - (returns) xmalloc'd buffer
//  [dstl]  - (returns) size of buffer
//  path    - path to file
//
// VARIANTS
//  *at
//   [dirfd] - relative path resolution dir
//
//  u* - only fails when errno != ENOENT
//
xapi snarfs(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path)
  __attribute__((nonnull(1, 3)));

xapi snarff(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 3)))
  __attribute__((format(printf, 3, 4)));

xapi snarfvf(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 3)));

xapi snarfats(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path)
  __attribute__((nonnull(1, 4)));

xapi snarfatf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 4)))
  __attribute__((format(printf, 4, 5)));

xapi snarfatvf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 4)));

xapi usnarfs(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path)
  __attribute__((nonnull(1, 3)));

xapi usnarff(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 3)))
  __attribute__((format(printf, 3, 4)));

xapi usnarfvf(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 3)));

xapi usnarfats(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path)
  __attribute__((nonnull(1, 4)));

xapi usnarfatf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, ...)
  __attribute__((nonnull(1, 4)))
  __attribute__((format(printf, 4, 5)));

xapi usnarfatvf(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 4)));

xapi fsnarf(char ** const restrict dst, size_t * const restrict dstl, int fd)
  __attribute__((nonnull));

#endif
