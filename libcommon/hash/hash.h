/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

uint32_t strhash32(uint32_t h, const char * restrict s)
  __attribute__((nonnull));

uint64_t strhash64(uint64_t h, const char * restrict s)
  __attribute__((nonnull));

/// hash
//
// SUMMARY
//  simple hash
//
uint32_t hash32(uint32_t h, const void * const restrict v, size_t l)
  __attribute__((nonnull));

/// hash
//
// SUMMARY
//  simple hash
//
uint64_t hash64(uint64_t h, const void * const restrict v, size_t l)
  __attribute__((nonnull));
