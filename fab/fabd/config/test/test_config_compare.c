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
#include "lorien/load.h"
#include "yyutil/load.h"
#include "logger/load.h"
#include "value/load.h"

#include "valyria/map.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "config_parser.h"
#include "config.internal.h"
#include "request.h"
#include "fsent.h"
#include "logging.h"

#include "yyutil/box.h"

typedef struct {
  XUNITTEST;

  char ** Atexts;
  char ** Btexts;

  bool build_changed;
  bool extern_changed;
  bool filesystems_changed;
  bool formula_changed;
  bool logging_changed;
} config_compare_test;

static xapi config_compare_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);
  fatal(config_setup);

  finally : coda;
}

static xapi config_compare_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);
  fatal(config_cleanup);

  finally : coda;
}

static xapi config_compare_test_entry(xunit_test * _test)
{
  enter;

  config_compare_test * test = containerof(_test, config_compare_test, xu);

  config_parser * parser = 0;
  configblob * A = 0;
  configblob * B = 0;
  configblob * T = 0;
  char ** text = 0;

  // arrange
  fatal(config_parser_create, &parser);

  // act - initial parse
  text = test->Atexts;
  while(*text)
  {
    fatal(config_parser_parse, parser, *text, strlen(*text) + 2, 0, 0, &T);
    if(!A)
    {
      A = T;
    }
    else
    {
      fatal(config_merge, A, T);
      fatal(config_xfree, T);
    }
    T = 0;
    text++;
  }

  text = test->Btexts;
  while(*text)
  {
    fatal(config_parser_parse, parser, *text, strlen(*text) + 2, 0, 0, &T);
    if(!B)
    {
      B = T;
    }
    else
    {
      fatal(config_merge, B, T);
      fatal(config_xfree, T);
    }
    T = 0;
    text++;
  }

  config_compare(B, A);

  assert_eq_b(test->build_changed, B->build.changed);
  assert_eq_b(test->extern_changed, B->extern_section.changed);

finally:
  fatal(config_parser_xfree, parser);
  fatal(config_xfree, A);
  fatal(config_xfree, B);
  fatal(config_xfree, T);
coda;
}

//
// manifest
//

xunit_unit xunit = {
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
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "extern : {"
                " bar"
                "}"
                "\0\0"
              }
            , 0
          }
        , extern_changed : true
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "extern : {"
                " bar"
                "}"
                "\0\0"
              }
            , (char[]) {
                "extern = {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
      }}
    , (config_compare_test[]) {{
          Atexts : (char*[]) {
              (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
        , Btexts : (char*[]) {
              (char[]) {
                "extern : {"
                " bar"
                "}"
                "\0\0"
              }
            , (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , 0
          }
        , extern_changed : true
      }}
    , 0
  }
};
