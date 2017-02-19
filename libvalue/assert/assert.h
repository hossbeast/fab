/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _VALUE_ASSERT_H
#define _VALUE_ASSERT_H

#include "xunit/assert.h"

extern struct xunit_type * value_xunit_value;

#define assert_eq_value(expected, actual)   _assertion(value_xunit_value, XU_EQ, QUOTE(actual), expected, actual)
#define assert_null_value(actual)           _assertion(value_xunit_value, XU_NULL, QUOTE(actual), actual)
#define assert_notnull_value(actual)        _assertion(value_xunit_value, XU_NOTNULL, QUOTE(actual), actual)

#endif
