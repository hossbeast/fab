/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/// LSIGX / MSIGX
//
// SUMMARY
//  access the nth byte in ascending(LSIGX) or descending(MSIGX) order of significance
//
// PARAMETERS
//  x - size of the value
//  n - byte to access
//
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define LSIGX(l, x) (x)
# define MSIGX(l, x) ((l) - (x) - 1)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
# define LSIGX(l, x) ((l) - (x) - 1)
# define MSIGX(l, x) (l)
#else
# error unable to determine endianness
