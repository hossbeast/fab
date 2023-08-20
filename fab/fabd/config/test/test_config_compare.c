/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/load.h"
#include "bacon/load.h"

#include "valyria/map.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "config_parser.h"
#include "config.internal.h"
#include "request.h"
#include "fsent.h"

#include "yyutil/box.h"

typedef struct {
  XUNITTEST;

  char ** Atexts;
  char ** Btexts;

  bool build_changed;
  bool walker_changed;
  bool filesystems_changed;
  bool formula_changed;
  bool logging_changed;
} config_compare_test;

static void config_compare_test_unit_setup(xunit_unit * unit)
{
  bacon_load();
  config_setup();

  /* override first-reconfiguration logic */
  config_reconfigured = true;
}

static void config_compare_test_unit_cleanup(xunit_unit * unit)
{
  bacon_unload();
  config_cleanup();
}

static void config_compare_test_entry(xunit_test * _test)
{
  config_compare_test * test = containerof(_test, config_compare_test, xu);

  config_parser * parser = 0;
  configblob * A = 0;
  configblob * B = 0;
  configblob * T = 0;
  char ** text = 0;

  // arrange
  config_parser_create(&parser);

  // act - initial parse
  text = test->Atexts;
  while(*text)
  {
    config_parser_parse(parser, *text, strlen(*text) + 2, 0, 0, &T);
    if(!A)
    {
      A = T;
    }
    else
    {
      config_merge(A, T);
      config_xfree(T);
    }
    T = 0;
    text++;
  }

  text = test->Btexts;
  while(*text)
  {
    config_parser_parse(parser, *text, strlen(*text) + 2, 0, 0, &T);
    if(!B)
    {
      B = T;
    }
    else
    {
      config_merge(B, T);
      config_xfree(T);
    }
    T = 0;
    text++;
  }

  config_compare(B, A);

  assert_eq_b(test->build_changed, B->build.changed);
  assert_eq_b(test->walker_changed, B->walker.changed);

  config_parser_xfree(parser);
  config_xfree(A);
  config_xfree(B);
  config_xfree(T);
}

//
// manifest
//

static xunit_unit xunit = {
    xu_setup : config_compare_test_unit_setup
  , xu_cleanup : config_compare_test_unit_cleanup
  , xu_entry : config_compare_test_entry
  , xu_tests : (config_compare_test*[]) {
      /* build */
      (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 4"
                "}"
                "\0\0"
              }
            , 0
          }
        , build_changed : true
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "build = {"
                " concurrency : 4"
                "}"
                "\0\0"
              }
            , 0
          }
        , build_changed : true
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 4"
                "}"
                "\0\0"
              }
            , (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "build = {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , 0
          }
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /bar"
                "}}"
                "\0\0"
              }
            , 0
          }
        , walker_changed : true
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /bar"
                "}}"
                "\0\0"
              }
            , (char[]) {
                "walker : { include = {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /bar"
                "}}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /bar"
                "}}"
                "\0\0"
              }
            , (char[]) {
                "walker : { include = {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
        , walker_changed : true
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                " /bar"
                "}}"
                "\0\0"
              }
            , (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
              }
            , 0
          }
        , walker_changed : true
      }}
    , 0
  }
};
XUNIT_UNIT(xunit);
