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

#include <inttypes.h>

#include "xapi.h"
#include "xlinux.h"
#include "narrator.h"
#include "narrator/file.h"

struct pathtree_test;
#define TEST_TYPE struct pathtree_test
#include "xunit.h"
#include "xunit/assert.h"

#include "valyria/list.h"

#define PATHTREE_VALUE_TYPE char *
#include "pathtree.internal.h"
#include "pathtree_test.h"

/*

TEST
  test_update

SUMMARY
  verifies the ability to update the value associated with an existing key

*/

static xapi pathtree_update_test_entry(pathtree_test * test)
{
  enter;

  pathtree * pt = 0;
  fatal(pathtree_create, &pt);

  // build the tree
  fatal(pathtree_insert, pt, "/gondor/moria", (char*[]) { "foo" });
  fatal(pathtree_insert, pt, "/gondor/moria/mordor", (char*[]) { "foo" });
  fatal(pathtree_insert, pt, "/gondor/moria/lorien", (char*[]) { "foo" });
  fatal(pathtree_insert, pt, "/gondor/moria/rohan", (char*[]) { "foo" });
  fatal(pathtree_insert, pt, "/gondor/moria/orthanc", (char*[]) { "foo" });
  fatal(pathtree_insert, pt, "/gondor/moria/osgiliath", (char*[]) { "foo" });

  // update an existing key
  fatal(pathtree_insert, pt, "/gondor/moria/osgiliath", (char*[]) { "bar" });

  fatal(pathtree_validate, pt->root, 0);

  char ** val = pathtree_search(pt, "/gondor/moria/osgiliath");

  assertf(strcmp("bar", val ? *val : "(none)") == 0, "%s", "%s", "bar", val ? *val : "(none)");

finally:
  pathtree_free(pt);
coda;
}

xunit_unit xunit = {
    tests : (xunit_test*[]) {
      (pathtree_test[]){{ entry : pathtree_update_test_entry }}
    , 0
  }
};
