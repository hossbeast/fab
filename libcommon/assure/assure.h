/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/// assure
//
// SUMMARY
//  reallocate if needed, given a size requirement
//
// PARAMETERS
//  target -
//  es     - element size
//  len    - required number of elements
//  ac     - pointer to allocated size in elements
//
void assure(void * target, size_t es, size_t len, size_t * const restrict ac);

/// assurex
//
// SUMMARY
//  assure with the ability to specify the seed
//
void assurex(void * target, size_t es, size_t len, size_t * const restrict ac, size_t seed);
