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

#include "xapi.h"

/// snarf
//
// SUMMARY
//  load the contents of a file into a buffer with read
//
// PARAMETERS
//  dst     - (returns) xmalloc'd buffer
//  [dstl]  - (returns) size of buffer
//  [dirfd] - relative path resolution dir
//  path    - path to file
//
xapi snarfs(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path)
  __attribute__((nonnull(1)));

xapi dsnarfs(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path)
  __attribute__((nonnull(1)));

/// usnarf
//
// SUMMARY
//  wrapper for snarf that only fails when errno != ENOENT
//
xapi usnarfs(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict path)
  __attribute__((nonnull(1)));

xapi udsnarfs(char ** const restrict dst, size_t * const restrict dstl, int dirfd, const char * const restrict path)
  __attribute__((nonnull(1)));
