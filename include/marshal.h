/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef _MARSHAL_H
#define _MARSHAL_H

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "macros.h"

/* marshal */

static inline size_t marshal_int(void * restrict dst, size_t sz, void *x, uint8_t xsz)
{
  uint32_t pad;

  pad = alignp(dst, xsz);
  RUNTIME_ASSERT(sz >= (pad + xsz));

  memcpy(dst + pad, x, xsz);
  return pad + xsz;
}

static inline size_t marshal_u8(void * restrict dst, size_t sz, uint8_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_u16(void * restrict dst, size_t sz, uint16_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_u32(void * restrict dst, size_t sz, uint32_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_u64(void * restrict dst, size_t sz, uint64_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_i8(void * restrict dst, size_t sz, int8_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_i16(void * restrict dst, size_t sz, int16_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_i32(void * restrict dst, size_t sz, int32_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

static inline size_t marshal_i64(void * restrict dst, size_t sz, int64_t x)
{
  return marshal_int(dst, sz, &x, sizeof(x));
}

/* unmarshal */

static inline size_t unmarshal_u32(void * restrict src, size_t sz, uint32_t *xp)
{
  uint32_t pad;

  pad = alignp(src, sizeof(*xp));

  memcpy(xp, src + pad, sizeof(*xp));
  return pad + sizeof(*xp);
}

#endif
