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

#include "rebase.h"

struct rebase_test;
#define TEST_TYPE struct rebase_test
#include "xunit.h"
#include "xunit/logging.h"
#include "xunit/XUNIT.errtab.h"

#include "logger.h"

typedef struct rebase_test
{
  xunit_test;
  char *    path;
  char *    base;
  char *    expected;
} rebase_test;

xapi rebase_test_entry(rebase_test * test)
{
  enter;

  char space[512];

  // log the inset
  logs(L_INSET, test->path);

  // transform
  fatal(rebase, test->path, 0, test->base, 0, space, sizeof(space), 0);

  // log the outset
  logs(L_OUTSET, space);

  if(strcmp(test->expected, space))
  {
    xapi_fail_intent();
    xapi_info_addf("expected", "%s", test->expected);
    xapi_info_addf("actual", "%s", space);
    tfail(perrtab_XUNIT, XUNIT_FAIL);
  }

  finally : coda;
}

xunit_unit xunit = {
    .tests = (xunit_test*[]) {
        (rebase_test[]){{ .entry = rebase_test_entry
          , path      : "/home/todd/fab/foo/bar"
          , base      : "/home/todd/fab"
          , expected  : "foo/bar"
        }}
      , (rebase_test[]){{ .entry = rebase_test_entry
          , path      : "/home/fab/foo/bar"
          , base      : "/home/todd/fab"
          , expected  : "../../fab/foo/bar"
        }}
      , (rebase_test[]){{ .entry = rebase_test_entry
          , path      : "/../foo/bar/baz"
          , base      : "/.."
          , expected  : "foo/bar/baz"
        }}
      , 0
  }
};
