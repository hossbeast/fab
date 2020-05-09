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

#include "xapi.h"
#include "xapi/errtab.h"

#include "xunit.h"
#include "xunit/assert.h"

#include "path.h"
#include "macros.h"

#define istrlen(a) a ? strlen(a) : 0

typedef struct path_test {
  XUNITTEST;

  char * input;
  char * path;
  char * dir;
  char * name;
  char * ext;
  char * suffix;
  char * base;
  char * stem;
} path_test;

static xapi path_test_entry(xunit_test * _test)
{
  enter;

  path_test * test = containerof(_test, path_test, xu);

  path * p;
  fatal(path_creates, &p, test->input);

  assert_eq_w(test->path, istrlen(test->path), p->path, p->pathl);
  assert_eq_w(test->dir, istrlen(test->dir), p->dir, p->dirl);
  assert_eq_w(test->name, istrlen(test->name), p->name, p->namel);
  assert_eq_w(test->ext, istrlen(test->ext), p->ext, p->extl);
  assert_eq_w(test->suffix, istrlen(test->suffix), p->suffix, p->suffixl);
  assert_eq_w(test->base, istrlen(test->base), p->base, p->basel);
  assert_eq_w(test->stem, istrlen(test->stem), p->stem, p->steml);

finally:
  path_free(p);
coda;
}

//
// public
//

xunit_unit xunit = {
    xu_entry : path_test_entry
  , xu_tests : (path_test*[]) {
      (path_test[]){{
          input : "/foo/bar.baz"
        , path : "/foo/bar.baz"
        , dir : "/foo"
        , name : "bar.baz"
        , ext : "baz"
        , suffix : "baz"
        , base : "bar"
        , stem : "/foo/bar"
      }}
    , (path_test[]){{
          input : "./foo/bar.baz.qux"
        , path : "./foo/bar.baz.qux"
        , dir : "./foo"
        , name : "bar.baz.qux"
        , ext : "baz.qux"
        , suffix : "qux"
        , base : "bar"
        , stem : "./foo/bar"
      }}
    , (path_test[]){{
          input : "./foo/bar/baz/qux"
        , path : "./foo/bar/baz/qux"
        , dir : "./foo/bar/baz"
        , name : "qux"
        , base : "qux"
        , stem : "./foo/bar/baz/qux"
      }}

    /* internal double-dot */
    , (path_test[]){{
          input : "./bar.qux"
        , path : "./bar.qux"
        , dir : "."
        , name : "bar.qux"
        , ext : "qux"
        , suffix : "qux"
        , base : "bar"
        , stem : "./bar"
      }}
    , (path_test[]){{
          input : "./bar.baz.qux"
        , path : "./bar.baz.qux"
        , dir : "."
        , name : "bar.baz.qux"
        , ext : "baz.qux"
        , suffix : "qux"
        , base : "bar"
        , stem : "./bar"
      }}
    , (path_test[]){{
          input : "./../bar"
        , path : "./../bar"
        , dir : "./.."
        , name : "bar"
        , base : "bar"
        , stem : "./../bar"
      }}

    /* relative paths */
    , (path_test[]){{
          input : "./bar.baz.qux"
        , path : "./bar.baz.qux"
        , dir : "."
        , name : "bar.baz.qux"
        , ext : "baz.qux"
        , suffix : "qux"
        , base : "bar"
        , stem : "./bar"
      }}
    , (path_test[]){{
          input : "./../bar.baz.qux"
        , path : "./../bar.baz.qux"
        , dir : "./.."
        , name : "bar.baz.qux"
        , ext : "baz.qux"
        , suffix : "qux"
        , base : "bar"
        , stem : "./../bar"
      }}

    /* absolute paths */
    , (path_test[]){{
          input : "/bar/baz.c"
        , path : "/bar/baz.c"
        , dir : "/bar"
        , name : "baz.c"
        , suffix : "c"
        , ext : "c"
        , base : "baz"
        , stem : "/bar/baz"
      }}
    , (path_test[]){{
          input : "/baz.c"
        , path : "/baz.c"
        , dir : "/"
        , name : "baz.c"
        , suffix : "c"
        , ext : "c"
        , base : "baz"
        , stem : "/baz"
      }}
    , (path_test[]){{
          input : "/bar/tez"
        , path : "/bar/tez"
        , dir : "/bar"
        , name : "tez"
        , base : "tez"
        , stem : "/bar/tez"
      }}
    , (path_test[]){{
          input : "/tez"
        , path : "/tez"
        , dir : "/"
        , name : "tez"
        , base : "tez"
        , stem : "/tez"
      }}

    /* nofile cases */
    , (path_test[]){{
          input : "/../bar.baz.qux"
        , path : "/../bar.baz.qux"
        , dir : "/.."
        , name : "bar.baz.qux"
        , ext : "baz.qux"
        , suffix : "qux"
        , base : "bar"
        , stem : "/../bar"
      }}
    , (path_test[]){{
          input : "/../bar"
        , path : "/../bar"
        , dir : "/.."
        , name : "bar"
        , base : "bar"
        , stem : "/../bar"
      }}

    /* the root */
    , (path_test[]){{
          input : "/"
        , path : "/"
        , dir : "/"
        , stem : "/"
      }}

    /* immediate paths */
    , (path_test[]){{
          input : "test"
        , path : "./test"
        , dir : "."
        , name : "test"
        , base : "test"
        , stem : "./test"
      }}
    , 0
    }
};
