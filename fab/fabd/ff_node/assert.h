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

#ifndef FABD_FF_NODE_ASSERT_H
#define FABD_FF_NODE_ASSERT_H

#include "xunit/assert.h"

/* assertions for fabfile node type */
extern struct xunit_type * xunit_ffn_type;

#define assert_eq_ffn_type(expected, actual)   _assertion(xunit_ffn_type, XU_EQ, QUOTE(actual), expected, actual)

#endif
