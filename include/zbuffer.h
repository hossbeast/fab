/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "types.h"
#include "macros.h"

/*
*
* NOTE there's an inconsistency in this module. The functions which are implemented with
* snprintf always append a null byte, but the others do not.
*
*/

/*
 * like snprintf except the following idiom will not overflow
 *
 * z += znloadf(dst + z, sz - z, fmt, ...)
 */
static inline size_t
  __attribute__((nonnull(1, 3)))
  __attribute__((format(printf, 3, 4)))
znloadf(void * restrict dst, size_t sz, const char * restrict fmt, ...)
{
  size_t z = 0;
  va_list va;

  if(sz) {
    va_start(va, fmt);
    z = MIN(sz - 1, vsnprintf(dst, sz, fmt, va));
    va_end(va);
  }

  return z;
}


/* like znloadf, except takes format string and va_list */
static inline size_t znloadvf(void * restrict dst, size_t sz, const char * restrict fmt, va_list va)
{
  size_t z = 0;

  if(sz) {
    z = MIN(sz - 1, vsnprintf(dst, sz, fmt, va));
  }

  return z;
}

/*
 * like znloadf, except takes a pointer / length pair
 *
 * dst - buffer to write to
 * sz  - size of dst
 * b   - pointer to bytes to write
 * bz  - number of bytes to write
 */
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
