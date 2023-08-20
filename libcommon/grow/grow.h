/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/// grow
//
// SUMMARY
//
// PARAMETERS
//  target -
//  es     - element size
//  len    - additional space in elements
//  ec     - used size in elements
//  ac     - pointer to allocated size in elements
//
void grow(void * target, size_t es, size_t len, size_t ec, size_t * const restrict ac);
