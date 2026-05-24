/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator/growing.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "moria/load.h"

#include "graph.h"
#include "moria.h"
#include "parser.internal.h"
#include "dispatch.h"


typedef struct moria_parser_test {
  XUNITTEST;

  char * graph;

  char * expected;
  int expected_exit;
} moria_parser_test;

//
// public
//

static void moria_parser_unit_setup(xunit_unit * unit)
{
  // load libraries
  moria_load();
}

static void moria_parser_unit_cleanup(xunit_unit * unit)
{
  moria_unload();
}

//
// tests
//

static void moria_parser_test_entry(xunit_test * _test)
{
  moria_parser_test * test = (typeof(test))_test;

  moria_graph g;
  moria_parser * p = 0;
  narrator_growing * N1 = 0;
  const char * actual;
  size_t actual_len;
  int exit;

  // arrange
  moria_graph_init(&g);
  moria_parser_create(&p, &g, 0, 0, moria_operations_dispatch, 0, 0);

  // act
  exit = moria_parser_graph_parse(p, MMS(test->graph));

  // assert
  assert_eq_d(test->expected_exit, exit);
  if(test->expected)
  {
    narrator_growing_create(&N1);
    moria_graph_say(&g, (llist *[]) { &p->vertices }, 1, (llist *[]) { &p->edges }, 1, 0, 0, 0, 0, 0, &N1->base);
    actual = N1->s;
    actual_len = N1->l;
    assert_eq_w(test->expected, strlen(test->expected), actual, actual_len);
  }

  moria_graph_destroy(&g);
  narrator_growing_free(N1);
  moria_parser_free(p);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : moria_parser_unit_setup
  , xu_cleanup : moria_parser_unit_cleanup
  , xu_entry : moria_parser_test_entry
  , xu_tests : (void*)(moria_parser_test*[]) {
      // unique labels
      (moria_parser_test[]) {{
          graph    : "A:B"
        , expected : "1-A 2-B 1:2"
      }}
    , (moria_parser_test[]) {{
          graph    : "A:B C:D E:F"
        , expected : "1-A 2-B 3-C 4-D 5-E 6-F 1:2 3:4 5:6"
      }}

      // unique labels, attrs
    , (moria_parser_test[]) {{
          graph    : "A!0x42:B"
        , expected : "1-A!0x42 2-B 1:2"
      }}
    , (moria_parser_test[]) {{
          graph    : "A:B!0x4"
        , expected : "1-A 2-B!0x4 1:2"
      }}
    , (moria_parser_test[]) {{
          graph    : "A:0x42:B"
        , expected : "1-A 2-B 1:0x42:2"
      }}

      // non unique labels
    , (moria_parser_test[]) {{
          graph    : "42-B!0x1:72-B!0x2"
        , expected : "1-B!0x1 2-B!0x2 1:2"
      }}

      // not unique labels, identity
    , (moria_parser_test[]) {{
          graph    : " 1-A 2-b 3-b"
                     " 1:0x40000000:2 1:0x1:3"
        , expected :  "1-A 2-b 3-b"
                     " 1:0x1:3 1:0x40000000:2"
      }}

      // hyper-edges
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C"
                      " 1:0x80000000:2,3"
        , expected  :  "1-A 2-B 3-C"
                      " 1:0x80000000:2,3"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C"
                      " 1:0x80420000:2,3"
        , expected  :  "1-A 2-B 3-C"
                      " 1:0x80420000:2,3"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1:0x80000000:2,3,4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1:0x80000000:2,3,4"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C"
                      " 1,2:0x80000000:3"
        , expected  :  "1-A 2-B 3-C"
                      " 1,2:0x80000000:3"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2,3:0x80000000:4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2,3:0x80000000:4"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2,3:0x84200000:4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2,3:0x84200000:4"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2:0x80000000:3,4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2:0x80000000:3,4"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D"
                      " 1,2:0x84200000:3,4"
        , expected  :  "1-A 2-B 3-C 4-D"
                      " 1,2:0x84200000:3,4"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x80000000:4,5,6"
        , expected  :  "1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x80000000:4,5,6"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x84200000:4,5,6"
        , expected  :  "1-A 2-B 3-C 4-D 5-E 6-F"
                      " 1,2,3:0x84200000:4,5,6"
      }}
      // hyper-edges, zero-to-many
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B"
                      " 1,2:0x80000000:_"
        , expected  :  "1-A 2-B"
                      " 1,2:0x80000000:_"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B"
                      " _:0x80000000:1,2"
        , expected  :  "1-A 2-B"
                      " _:0x80000000:1,2"
      }}
    , (moria_parser_test[]){{
          graph     : " 1-A 2-B"
                      " _:0x80000000:1"
        , expected  :  "1-A 2-B"
                      " _:0x80000000:1"
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
