/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/// stresc
//
// SUMMARY
//  write a string to a buffer with all nonprinting characters and non-space whitespace replaced
//  with hex escape sequences
//
// PARAMETERS
//  dst - dst buffer
//  sz  - size of dst
//  src - source string
//  len - length of src
//
// RETURNS
//  returns number of bytes written
//
size_t strescw(void * restrict dst, size_t sz, const char * const restrict src, size_t len)
  __attribute__((nonnull(1, 3)));
