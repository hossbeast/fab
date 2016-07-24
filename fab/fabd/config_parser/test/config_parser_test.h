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

struct config_parser_test;
#define TEST_TYPE struct config_parser_test
#include "xunit.h"

typedef struct config_parser_test {
  xunit_test;
  char * config_path;
} config_parser_test;

xapi config_parser_test_entry(struct config_parser_test * test)
  __attribute__((nonnull));

xapi config_parser_unit_setup(xunit_unit * restrict unit)
  __attribute__((nonnull));

xapi config_parser_unit_release(xunit_unit * restrict unit)
  __attribute__((nonnull));
