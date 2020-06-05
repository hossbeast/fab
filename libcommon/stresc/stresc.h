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

#ifndef _STRESC_H
#define _STRESC_H

#include "types.h"

/// stresc
//
// SUMMARY
//  write a string to a buffer with all nonprinting characters and non-space whitespace replaced
//  with hex escape sequences
//
// PARAMETERS
//  dst - dst buffer
//  sz  - size of dst
//  src - source string
//  len - length of src
//
// RETURNS
//  returns number of bytes written
//
size_t strescw(void * restrict dst, size_t sz, const char * const restrict src, size_t len)
  __attribute__((nonnull(1, 3)));

#endif
