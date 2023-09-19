/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This name is part of fab.

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

#include <stdlib.h>


#include "xunit.h"
#include "xunit/assert.h"

#include "path_normalize.h"
#include "macros.h"

#define istrlen(a) a ? strlen(a) : 0

typedef struct path_test {
  XUNITTEST;

  char * input;
  char * output;
} path_test;

static void path_test_entry(xunit_test * _test)
{
  path_test * test = containerof(_test, path_test, xu);

  char space[512];

  size_t len = path_normalize(space, sizeof(space), test->input);

  assert_eq_w(test->output, istrlen(test->output), space, len);
}

//
// public
//

static xunit_unit xunit = {
    xu_entry : path_test_entry
  , xu_tests : (void*)(path_test*[]) {
      /* redundant slashes */
        (path_test[]){{
            input : "//foo"
          , output : "/foo"
        }}
      , (path_test[]){{
            input : "//foo//bar"
          , output : "/foo/bar"
        }}
      , (path_test[]){{
            input : "foo//bar"
          , output : "./foo/bar"
        }}

      /* internal double-dot */
      , (path_test[]){{
            input : "foo/../bar.qux"
          , output : "./bar.qux"
        }}
      , (path_test[]){{
            input : "alpha/omega/../../bar.baz.qux"
          , output : "./bar.baz.qux"
        }}
      , (path_test[]){{
            input : "alpha/omega/../../../bar"
          , output : "./../bar"
        }}
      , (path_test[]){{
            input : "./alpha/omega/../../../bar"
          , output : "./../bar"
        }}

      /* relative paths */
      , (path_test[]){{
            input : "./bar.baz.qux"
          , output : "./bar.baz.qux"
        }}
      , (path_test[]){{
            input : "./../bar.baz.qux"
          , output : "./../bar.baz.qux"
        }}
      , (path_test[]){{
            input : "foo/bar.baz.qux"
          , output : "./foo/bar.baz.qux"
        }}

      /* absolute paths */
      , (path_test[]){{
            input : "/foo/bar.baz"
          , output : "/foo/bar.baz"
        }}
      , (path_test[]){{
            input : "/bar/../baz.c"
          , output : "/baz.c"
        }}
      , (path_test[]){{
            input : "/bar/baz/../tez"
          , output : "/bar/tez"
        }}
      , (path_test[]){{
            input : "/bar/baz/../../tez"
          , output : "/tez"
        }}
      , (path_test[]){{
            input : "/bar/baz/../../../tez"
          , output : "/tez"
        }}

      /* nofile cases */
      , (path_test[]){{
            input : "/../bar.baz.qux"
          , output : "/../bar.baz.qux"
        }}
      , (path_test[]){{
            input : "/../../bar"
          , output : "/../bar"
        }}

      /* the root */
      , (path_test[]){{
            input : "/"
          , output : "/"
        }}
      , (path_test[]){{
            input : "//"
          , output : "/"
        }}
      , 0
    }
};
XUNIT_UNIT(xunit);
