/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>


#include "xunit.h"
#include "xunit/assert.h"

#include "fsent_name.h"
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

  fsent_name p;
  fsent_name_init(&p, MMS(test->input));

  assert_eq_w(test->name, istrlen(test->name), p.name, p.namel);
  assert_eq_w(test->ext, istrlen(test->ext), p.ext, p.extl);
  assert_eq_w(test->suffix, istrlen(test->suffix), p.suffix, p.suffixl);
  assert_eq_w(test->base, istrlen(test->base), p.base, p.basel);
}

//
// public
//

static xunit_unit xunit = {
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
XUNIT_UNIT(xunit);
