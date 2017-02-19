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

#ifndef _LORIEN_PATH_NORMALIZE_H
#define _LORIEN_PATH_NORMALIZE_H

#include <sys/types.h>

#define restrict __restrict

/// normalize
//
// SUMMARY
//  remove redundant dashes and dot-dot sequences from a path
//
// PARAMETERS
//  dst - location to write the normalized path to
//  sz  - size of dst >= strlen(s) + 2
//  s   - path to normalize
//
// RETURNS
//  number of bytes written to dst
//
size_t path_normalize(char * restrict dst, size_t sz, const char * restrict path)
  __attribute__((nonnull));

#undef restrict
#endif
