/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

// ca - count all
// cl - count leading
// ct - count trailing

static inline int caones(unsigned int x)
{
  return __builtin_popcount(x);
}
