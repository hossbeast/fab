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

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "rebase.internal.h"

struct rebase_test;
#define TEST_TYPE struct rebase_test
#include "xunit.h"
#include "xunit/assert.h"

typedef struct rebase_test
{
  xunit_test;
  char *    path;
  char *    base;
  char *    expected;
} rebase_test;

static xapi rebase_test_entry(rebase_test * test)
{
  enter;

  char space[512];

  // transform
  fatal(rebase, test->path, 0, test->base, 0, space, sizeof(space), 0);

  assert_eq_s(test->expected, space);

  finally : coda;
}

xunit_unit xunit = {
    .xu_entry = rebase_test_entry
  , .xu_tests = (xunit_test*[]) {
        (rebase_test[]){{
            path      : "/home/todd/fab/foo/bar"
          , base      : "/home/todd/fab"
          , expected  : "foo/bar"
        }}
      , (rebase_test[]){{
            path      : "/home/fab/foo/bar"
          , base      : "/home/todd/fab"
          , expected  : "../../fab/foo/bar"
        }}
      , (rebase_test[]){{
            path      : "/../foo/bar/baz"
          , base      : "/.."
          , expected  : "foo/bar/baz"
        }}
      , 0
  }
};
