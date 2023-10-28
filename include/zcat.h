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
 * z += zcatf(dst + z, sz - z, fmt, ...)
 */
static inline size_t
  __attribute__((nonnull(1, 3)))
  __attribute__((format(printf, 3, 4)))
zcatf(void * restrict dst, size_t sz, const char * restrict fmt, ...)
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


/* like zcatf, except takes format string and va_list */
static inline size_t zcatvf(void * restrict dst, size_t sz, const char * restrict fmt, va_list va)
{
  size_t z = 0;

  if(sz) {
    z = MIN(sz - 1, vsnprintf(dst, sz, fmt, va));
  }

  return z;
}

/*
 * like zcatf, except takes a pointer / length pair
 *
 * dst - buffer to write to
 * sz  - size of dst
 * b   - pointer to bytes to write
 * bz  - number of bytes to write
 */
static inline size_t zcatw(void * restrict dst, size_t sz, const void * b, size_t bz)
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

/* zcats - like zcatf, except takes a pointer to string */
#define zcats(dst, sz, s) zcatw(dst, sz, s, strlen(s))

/* zcatc - like zcatf, except takes a char */
#define zcatc(dst, sz, c) zcatw(dst, sz, (char[]) { c }, 1)
