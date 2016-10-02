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

#ifndef _LISTWISE_TEST_H
#define _LISTWISE_TEST_H

#include "xapi.h"

struct listwise_test;
#define TEST_TYPE struct listwise_test
#include "xunit.h"
#include "xunit/assert.h"

xapi listwise_test_setup(xunit_unit *);
xapi listwise_test_cleanup(xunit_unit *);
xapi listwise_test_entry(struct listwise_test*)
  __attribute__((nonnull));

typedef struct listwise_test
{
  xunit_test;
  char ** init;
  char * xsfm;
  char ** final;
} listwise_test;

#endif
