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

#include "fsname.h"
#include "macros.h"

#define istrlen(a) a ? strlen(a) : 0

typedef struct path_test {
  XUNITTEST;

  char * input;
  char * name;
  char * ext;
  char * suffix;
  char * base;
} path_test;

static void path_test_entry(xunit_test * _test)
{
  path_test * test = containerof(_test, path_test, xu);

  fsname p;
  fsname_init(&p, MMS(test->input));

  assert_eq_w(test->name, istrlen(test->name), p.name, p.namel);
  assert_eq_w(test->ext, istrlen(test->ext), p.ext, p.extl);
  assert_eq_w(test->suffix, istrlen(test->suffix), p.suffix, p.suffixl);
  assert_eq_w(test->base, istrlen(test->base), p.base, p.basel);
}

//
// public
//

xunit_unit xunit = {
    xu_entry : path_test_entry
  , xu_tests : (path_test*[]) {
      (path_test[]){{
          input : "bar.baz.qux"
        , name : "bar.baz.qux"
        , ext : "baz.qux"
        , suffix : "qux"
        , base : "bar"
      }}
    , (path_test[]){{
          input : "baz.c"
        , name : "baz.c"
        , suffix : "c"
        , ext : "c"
        , base : "baz"
      }}
    , (path_test[]){{
          input : "tez"
        , name : "tez"
        , base : "tez"
      }}
    , 0
    }
};
