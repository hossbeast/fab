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

#include "xapi.h"
#include "valyria/load.h"
#include "moria/load.h"
#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "moria/graph.h"
#include "moria/operations.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "logging.h"
#include "rule.h"
#include "fsent.h"
#include "pattern.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "variant.h"

typedef struct variant_test {
  XUNITTEST;

  // inputs
  char * text;

  // outputs
  char * norm;
  char ** tags;
} variant_test;

static xapi variant_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(valyria_load);
  fatal(logging_finalize);

  fatal(variant_setup);

  finally : coda;
}

static xapi variant_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(valyria_unload);
  fatal(variant_cleanup);

  finally : coda;
}

//
// tests
//

static xapi variant_test_entry(xunit_test * _test)
{
  enter;

  variant_test * test = containerof(_test, variant_test, xu);
  variant *v;
  int x;

  fatal(variant_get, test->text, strlen(test->text), &v);

  assert_eq_w(test->norm, strlen(test->norm), v->norm, v->norm_len);
  assert_eq_zu(sentinel(test->tags), v->tags_len);
  for(x = 0; x < v->tags_len; x++)
    assert_eq_w(test->tags[x], strlen(test->tags[x]), v->tags[x].text, v->tags[x].len);

  finally : coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : variant_test_unit_setup
  , xu_cleanup : variant_test_unit_cleanup
  , xu_entry : variant_test_entry
  , xu_tests : (variant_test*[]) {
      (variant_test[]) {{
          text : "foo"
        , norm : "foo"
        , tags : (char*[]) { "foo", 0 }
      }}
    , (variant_test[]) {{
          text : "bar.foo"
        , norm : "bar.foo"
        , tags : (char*[]) { "bar", "foo", 0 }
      }}
    , (variant_test[]) {{
          text : "bar.foo"
        , norm : "bar.foo"
        , tags : (char*[]) { "bar", "foo", 0 }
      }}
    , (variant_test[]) {{
          text : ".foo"
        , norm : "foo"
        , tags : (char*[]) { "foo", 0 }
      }}
    , (variant_test[]) {{
          text : "foo."
        , norm : "foo"
        , tags : (char*[]) { "foo", 0 }
      }}
    , (variant_test[]) {{
          text : "foo..bar"
        , norm : "bar.foo"
        , tags : (char*[]) { "bar", "foo", 0 }
      }}
      /* redundant dots */
    , (variant_test[]) {{
          text : "foo...bar"
        , norm : "bar.foo"
        , tags : (char*[]) { "bar", "foo", 0 }
      }}
      /* exclude duplicates */
    , (variant_test[]) {{
          text : "foo.bar.foo"
        , norm : "bar.foo"
        , tags : (char*[]) { "bar", "foo", 0 }
      }}
    , 0
  }
};
