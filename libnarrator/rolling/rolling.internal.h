/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <fcntl.h>

#include "types.h"

#include "rolling.h"

/// rolling_say
//
// SUMMARY
//  write to a rolling_narrator
//
// PARAMETERS
//  n     - rolling narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
//

xapi rolling_xsayvf(narrator_rolling * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi rolling_xsayw(narrator_rolling * const restrict n, const char * const restrict b, size_t l);

/// rolling_seek
//
// SUMMARY
//  returns ENOTSUPP
//
xapi rolling_xseek(narrator_rolling * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull));

/// rolling_xdestroy
//
// SUMMARY
//  release resources owned by a rolling narrator, does not free n
//
xapi rolling_xdestroy(narrator_rolling * const restrict n)
  __attribute__((nonnull));

xapi rolling_xread(narrator_rolling * restrict n, void * dst, size_t count, size_t * restrict r)
  __attribute__((nonnull(1, 2)));

xapi rolling_flush(narrator_rolling * restrict n)
  __attribute__((nonnull));
