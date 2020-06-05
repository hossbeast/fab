/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef _COMMON_TYPES_H
#define _COMMON_TYPES_H

/*
 * common type definitions - suitable for inclusion anywhere in the fab project
 */

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#define restrict __restrict

static inline size_t roundup(size_t x, size_t y)
{
  size_t r = x % y;
  if(r)
    return x + (y - r);
  return x;
}

#define API
#define APIDATA

#endif
