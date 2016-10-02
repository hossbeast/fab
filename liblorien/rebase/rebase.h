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

#ifndef _LORIEN_REBASE_H
#define _LORIEN_REBASE_H

#include <sys/types.h>

#include "xapi.h"

#define restrict __restrict

/// rebase
//
// SUMMARY
//  reformulate a normalized absolute path into a relative path in terms of some absolute base path
//
// PARAMETERS
//  path      - absolute, normalized path to rebase
//  [pathl]   - length of path, 0 for strlen
//  base      - absolute directory path to rebase against
//  [basel]   - length of base, 0 for strlen
//  dst       - path relative to base is written here
//  siz       - max bytes to write to dst
//  [z]       - returns the number of bytes written
//
xapi rebase(
    /* 1 */ const char * const restrict path
  , /* 2 */ size_t pathl
  , /* 3 */ const char * const restrict base
  , /* 4 */ size_t basel
  , /* 5 */ char * const restrict dst
  , /* 6 */ const size_t siz
  , /* 7 */ size_t * restrict z
)
  __attribute__((nonnull(1, 3, 5)));

#undef restrict
#endif
