/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

extern struct xunit_type * xunit_strht;

#define assert_eq_strht(expected, actual) _assertion(xunit_strht, XU_EQ, QUOTE(actual), expected, actual)

uint32_t strht_hash(uint32_t h, const void *);
int strht_cmp(const void *, const void *);
