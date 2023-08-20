/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "fixed.h"

/// narrator_fixed_say
//
// SUMMARY
//  write to the narrator
//
// PARAMETERS
//  n     - fixed narrator
//  [fmt] - printf-style format string
//  [va]  - varargs
//  [b]   - buffer
//  [l]   - size of buffer
//
// RETURNS
//  returns the number of bytes written
//
int fixed_sayvf(narrator_fixed * restrict n, const char * restrict fmt, va_list va)
  __attribute__((nonnull));

int fixed_sayw(narrator_fixed * restrict n, const char * restrict b, size_t l);

/// narrator_fixed_seek
//
// SUMMARY
//  reposition the narrator to offset according to whence
//
// PARAMETERS
//  n      - fixed narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
off_t fixed_seek(narrator_fixed * restrict n, off_t offset, int whence)
  __attribute__((nonnull));

off_t fixed_reset(narrator_fixed * restrict n)
  __attribute__((nonnull));

/// narrator_fixed_read
//
// SUMMARY
//  copy bytes from the underlying buffer and reposition
//
// PARAMETERS
//  n     - fixed narrator
//  dst   - destination buffer
//  count - number of bytes to copy
//
// RETURNS
//  number of bytes actually copied <= count
//
size_t fixed_read(narrator_fixed * restrict n, void * dst, size_t count)
  __attribute__((nonnull));

void fixed_flush(narrator_fixed * restrict n)
  __attribute__((nonnull));
