/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "macros.h"

/*
*
* NOTE there's an inconsistency in this module. The functions which are implemented with
* snprintf always append a null byte, but the others do not.
*
*/

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

/// znloadvf
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
static inline size_t znloadw(void * restrict dst, size_t sz, const void * b, size_t bz)
{
  size_t z = 0;

  if(sz) {
    z = MIN(sz - 1, bz); // leave room for a trailing null
  }
  if(z) {
    memcpy(dst, b, z);
  }
  return z;
}

/// znloads
//
// SUMMARY
//  like znloadf, except takes a pointer to string
//
#define znloads(dst, sz, s) znloadw(dst, sz, s, strlen(s))
#define znloadc(dst, sz, c) znloadw(dst, sz, (char[]) { c }, 1)

static inline size_t znload_int(void * restrict dst, size_t sz, void *x, uint8_t xsz)
{
  uint8_t pad;

  pad = ((uintptr_t)dst) & (xsz - 1);

  if(sz < pad) {
    return 0;
  }

  return pad + memncpy(dst + pad, sz - pad, x, xsz);
}

static inline size_t znload_u8(void * restrict dst, size_t sz, uint8_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_u16(void * restrict dst, size_t sz, uint16_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_u32(void * restrict dst, size_t sz, uint32_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_u64(void * restrict dst, size_t sz, uint64_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_i8(void * restrict dst, size_t sz, int8_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_i16(void * restrict dst, size_t sz, int16_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_i32(void * restrict dst, size_t sz, int32_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}

static inline size_t znload_i64(void * restrict dst, size_t sz, int64_t x)
{
  return znload_int(dst, sz, &x, sizeof(x));
}
