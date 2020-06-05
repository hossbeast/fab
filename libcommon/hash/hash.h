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

#ifndef _HASH_H
#define _HASH_H

#include "types.h"

uint32_t strhash32(uint32_t h, const char * restrict s)
  __attribute__((nonnull));

uint64_t strhash64(uint64_t h, const char * restrict s)
  __attribute__((nonnull));

/// hash
//
// SUMMARY
//  simple hash
//
uint32_t hash32(uint32_t h, const void * const restrict v, size_t l)
  __attribute__((nonnull));

/// hash
//
// SUMMARY
//  simple hash
//
uint64_t hash64(uint64_t h, const void * const restrict v, size_t l)
  __attribute__((nonnull));

#endif
