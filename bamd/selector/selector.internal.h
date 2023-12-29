/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "selector.h"

/* unit test only assertion stuff */
extern struct xunit_type * xunit_selector;

#define assert_eq_selector(expected, actual)   _assertion(xunit_selector, XU_EQ, QUOTE(actual), expected, actual)
#define assert_null_selector(actual)           _assertion(xunit_selector, XU_NULL, QUOTE(actual), actual)
#define assert_notnull_selector(actual)        _assertion(xunit_selector, XU_NOTNULL, QUOTE(actual), actual)
