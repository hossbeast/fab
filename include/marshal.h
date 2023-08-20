/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
