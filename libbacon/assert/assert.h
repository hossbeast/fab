/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "xunit/assert.h"

extern struct xunit_type * value_xunit_value;

#define assert_eq_value(expected, actual)   _assertion(value_xunit_value, XU_EQ, QUOTE(actual), expected, actual)
#define assert_null_value(actual)           _assertion(value_xunit_value, XU_NULL, QUOTE(actual), actual)
#define assert_notnull_value(actual)        _assertion(value_xunit_value, XU_NOTNULL, QUOTE(actual), actual)
