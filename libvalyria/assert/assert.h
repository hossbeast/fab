/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "xunit/assert.h"

extern struct xunit_type * valyria_xunit_set;

#define assert_eq_set(expected, actual)   _assertion(valyria_xunit_set, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_set(expected, actual)   _assertion(valyria_xunit_set, XU_EQ, QUOTE(actual), expected, actual)
