/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "bacon/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"


#include "value.h"
#include "assert.h"
#include "writer.h"
#include "builder.h"

#include "macros.h"
#include "common/attrs.h"

typedef struct writer_test {
  XUNITTEST;
  void (*say)(bacon_writer *);
  bool singleline;
  const char *expected;
} writer_test;

static void say_set_single(bacon_writer * restrict builder_writer)
{
  { SET
    STRING("hi")
    STRING("there")
  }
}

static void say_set_multi(bacon_writer * restrict builder_writer)
{
  { SET
    { SET
      STRING("hi")
      STRING("there")
    }
  }
}

static void say_list_set_mapping(bacon_writer * restrict builder_writer)
{
  { LIST
    { SET
      { MAPPING
        STRING("hi")
        STRING("there")
      }
    }
    { SET
      { MAPPING
        STRING("hi")
        STRING("there")
      }
    }
  }
}

static void say_set_mapping_list_set(bacon_writer * restrict builder_writer)
{
  { SET
    { MAPPING
      STRING("list")
      { LIST
        { SET
          STRING("hi")
        }
        { SET
          STRING("hi")
        }
      }
    }
  }
}

static void say_set_mapping_key_empty(bacon_writer * restrict builder_writer)
{
  { SET
    { MAPPING
      STRING("")      // since its part of a mapping, nonempty string is substituted
      STRING("A")
    }
    { MAPPING
      STRING("list")
      { LIST
        { SET
          STRING("hi")
        }
        { SET
          STRING("hi")
        }
      }
    }
  }
}

static void say_set_mapping_val_empty(bacon_writer * restrict builder_writer)
{
  { SET
    { MAPPING
      STRING("A")
      STRING("")      // since its part of a mapping, nonempty string is substituted
    }
    { MAPPING
      STRING("list")
      { LIST
        { SET
          STRING("hi")
        }
        { SET
          STRING("hi")
        }
      }
    }
  }
}

static void say_mappings(bacon_writer * restrict builder_writer)
{
  { SET
    { MAPPING
      STRING("hi")
      STRING("there")
    }
  }
}

static void bacon_writer_unit_setup(xunit_unit * unit)
{
  bacon_load();
}

static void bacon_writer_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
}

//
// tests
//

static void test_builder_entry(xunit_test * _test)
{
  writer_test * test = (void*)_test;
  narrator_growing *N;
  bacon_writer writer;
  bacon_writer *builder_writer;

  /* arrange */
  bacon_writer_init(&writer);
  narrator_growing_create(&N);
  builder_writer = &writer;
  bacon_writer_open(builder_writer, &N->base);

  /* act */
  writer.singleline = test->singleline;
  test->say(&writer);

  assert_eq_w(test->expected, strlen(test->expected), N->s, N->l);

  bacon_writer_destroy(&writer);
  narrator_growing_free(N);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : bacon_writer_unit_setup
  , xu_cleanup : bacon_writer_unit_cleanup
  , xu_entry : test_builder_entry
  , xu_tests : (void*)(writer_test*[]) {
        (writer_test[]){{
            .say = say_set_single
          , .singleline = true
          , .expected = "{ hi there }"
        }}
      , (writer_test[]){{
            .say = say_set_single
          , .singleline = false
          , .expected = "{\n  hi\n  there\n}"
        }}
      , (writer_test[]){{
            .say = say_set_multi
          , .singleline = true
          , .expected = "{ { hi there } }"
        }}
      , (writer_test[]){{
            .say = say_set_multi
          , .singleline = false
          , .expected = "{\n  {\n    hi\n    there\n  }\n}"
        }}
      , (writer_test[]){{
            .say = say_mappings
          , .singleline = true
          , .expected = "{ hi : there }"
        }}
      , (writer_test[]){{
            .say = say_mappings
          , .singleline = false
          , .expected = "{\n  hi : there\n}"
        }}
      , (writer_test[]){{
            .say = say_list_set_mapping
          , .singleline = true
          , .expected = "[ { hi : there } { hi : there } ]"
        }}
      , (writer_test[]){{
            .say = say_set_mapping_list_set
          , .singleline = true
          , .expected = "{ list : [ { hi } { hi } ] }"
        }}
      , (writer_test[]){{
            .say = say_set_mapping_key_empty
          , .singleline = true
          , .expected = "{ _ : A list : [ { hi } { hi } ] }"
        }}
      , (writer_test[]){{
            .say = say_set_mapping_val_empty
          , .singleline = true
          , .expected = "{ A : _ list : [ { hi } { hi } ] }"
        }}
    , 0
  }
};
XUNIT_UNIT(xunit);
