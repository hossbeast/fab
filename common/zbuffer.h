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

#ifndef _ZBUFFER_H
#define _ZBUFFER_H

#include <stdio.h>
#include <string.h>

#include "macros.h"

#define restrict __restrict

/// znload
//
// SUMMARY
//  like snprintf except the following idiom will not overflow
//
// z += znloadf(dst + z, sz - z, fmt, ...)
//
#define znloadf(dst, sz, fmt, ...) ({                         \
  size_t __z = 0;                                             \
  if(sz)                                                      \
  {                                                           \
    __z = MIN(sz - 1, snprintf(dst, sz, fmt, ##__VA_ARGS__)); \
  }                                                           \
  __z;                                                        \
})

/// znvloadf
//
// SUMMARY
//  like znloadf, except takes format string and va_list
//
#define znloadvf(dst, sz, fmt, va) ({                 \
  size_t __z = 0;                                     \
  if(sz)                                              \
  {                                                   \
    __z = MIN(sz - 1, vsnprintf(dst, sz, fmt, va));   \
  }                                                   \
  __z;                                                \
})

/// znloadw
//
// SUMMARY
//   like znloadf, except takes a pointer / length pair
//
// PARAMETERS
//  dst - buffer to write to
//  sz  - size of dst
//  b   - pointer to bytes to write
//  bz  - number of bytes to write
//
#define znloadw(dst, sz, b, bz) ({            \
  size_t __z = MIN(sz - 1, bz);               \
  memcpy(dst, b, __z);                        \
  ((char*)dst)[__z] = 0;                      \
  __z;                                        \
})

/// znloads
//
// SUMMARY
//  like znloadf, except takes a pointer to string
//
#define znloads(dst, sz, s) znloadw(dst, sz, s, strlen(s)) 

/// zloads
//
//
//
static inline size_t zloads(void * restrict dst, const char * const restrict src)
{
  size_t len = strlen(src);
  memcpy(dst, src, len);
  return len;
}

#undef restrict
#endif
