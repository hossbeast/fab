/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

static void variant_test_unit_setup(xunit_unit * unit)
{

  variant_setup();
}

static void variant_test_unit_cleanup(xunit_unit * unit)
{
  variant_cleanup();
}

//
// tests
//

static void variant_test_entry(xunit_test * _test)
{
  variant_test * test = containerof(_test, variant_test, xu);
  variant *v;
  int x;

  variant_get(test->text, strlen(test->text), &v);

  assert_eq_w(test->norm, strlen(test->norm), v->norm, v->norm_len);
  assert_eq_zu(sentinel(test->tags), v->tags_len);
  for(x = 0; x < v->tags_len; x++)
    assert_eq_w(test->tags[x], strlen(test->tags[x]), v->tags[x].text, v->tags[x].len);
}

//
// manifest
//

static xunit_unit xunit = {
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
XUNIT_UNIT(xunit);
