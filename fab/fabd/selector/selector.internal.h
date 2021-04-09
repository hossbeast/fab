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

#ifndef _FABD_SELECTOR_INTERNAL_H
#define _FABD_SELECTOR_INTERNAL_H

#include "selector.h"

/* unit test only assertion stuff */
extern struct xunit_type * xunit_selector;

#define assert_eq_selector(expected, actual)   _assertion(xunit_selector, XU_EQ, QUOTE(actual), expected, actual)
#define assert_null_selector(actual)           _assertion(xunit_selector, XU_NULL, QUOTE(actual), actual)
#define assert_notnull_selector(actual)        _assertion(xunit_selector, XU_NOTNULL, QUOTE(actual), actual)

#endif
