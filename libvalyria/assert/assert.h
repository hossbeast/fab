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

#ifndef _VALYRIA_ASSERT_H
#define _VALYRIA_ASSERT_H

#include "xunit/assert.h"

extern struct xunit_type * valyria_xunit_set;

#define assert_eq_set(expected, actual)   _assertion(valyria_xunit_set, XU_EQ, QUOTE(actual), expected, actual)
#define assert_ne_set(expected, actual)   _assertion(valyria_xunit_set, XU_EQ, QUOTE(actual), expected, actual)

#endif
