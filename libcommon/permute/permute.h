/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/// permute
//
// SUMMARY
//  reorder a set in place to obtain the next permutation
//
// RETURNS
//  true if the next permutation exists
//
bool permute(uint8_t * const restrict set, size_t setl)
  __attribute__((nonnull));
