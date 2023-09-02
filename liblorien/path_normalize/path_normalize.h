/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/// normalize
//
// SUMMARY
//  remove redundant dashes and dot-dot sequences from a path
//
// PARAMETERS
//  dst   - location to write the normalized path to
//  sz    - size of dst >= len + 2
//  path  - path to normalize
//  len   - length of path
//
// RETURNS
//  number of bytes written to dst
//
size_t path_normalize(char * restrict dst, size_t sz, const char * restrict path, uint16_t len)
  __attribute__((nonnull));
