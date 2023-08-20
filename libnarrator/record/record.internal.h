/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "record.h"

/// record_say
//
// SUMMARY
//  accumulate to the buffer
//
xapi record_xsayvf(narrator_record * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

xapi record_xsayw(narrator_record * const restrict n, const char * const restrict b, size_t l);

/// record_seek
//
// SUMMARY
//  seek the underlying narrator
//
xapi record_xseek(narrator_record * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull));

/// record_destroy
//
// SUMMARY
//  destroy a record narrator
//
void record_destroy(narrator_record * const restrict n)
  __attribute__((nonnull));

xapi record_xread(narrator_record * restrict n, void * dst, size_t count, size_t * restrict r)
  __attribute__((nonnull(1, 2)));
