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

#include "lorien/load.h"
#include "yyutil/load.h"
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

#include "yyutil/box.h"

typedef struct {
  XUNITTEST;

  char ** texts;
  char * expected;
} config_merge_test;

static void config_merge_test_unit_setup(xunit_unit * unit)
{
  yyutil_load();
  value_load();
  logging_finalize();
  config_setup();
}

static void config_merge_test_unit_cleanup(xunit_unit * unit)
{
  yyutil_unload();
  value_unload();
  config_cleanup();
}

static void config_merge_test_entry(xunit_test * _test)
{
  config_merge_test * test = containerof(_test, config_merge_test, xu);

  config_parser * parser = 0;
  configblob * A = 0;
  configblob * B = 0;
  configblob * T = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  char ** text = 0;

  // arrange
  config_parser_create(&parser);
  narrator_growing_create(&N1);
  narrator_growing_create(&N2);

  // act - initial parse
  text = test->texts;
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

  config_parser_parse(parser, test->expected, strlen(test->expected) + 2, 0, 0, &B);

  config_say(A, &N1->base);
  config_say(B, &N2->base);
  assert_eq_w(N2->s, N2->l, N1->s, N1->l);

finally:
  config_parser_xfree(parser);
  config_xfree(A);
  config_xfree(B);
  config_xfree(T);
  narrator_growing_free(N1);
  narrator_growing_free(N2);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : config_merge_test_unit_setup
  , xu_cleanup : config_merge_test_unit_cleanup
  , xu_entry : config_merge_test_entry
  , xu_tests : (config_merge_test*[]) {
      /* build */
      (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 12"
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
        , expected : (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 12"
                "}"
                "\0\0"
              }
            , (char[]) {
                "build = {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "build : {"
                " concurrency : 42"
                "}"
                "\0\0"
              }
            , (char[]) {
                "build = {"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "\0\0"
          }
      }}

      /* walker */
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                "  /usr/lib/fab/builtin-goats"
                "  /home/todd/my/goats"
                "}}\0\0"
              }
            , (char[]) {
                "walker : { include :{"
                "  /foo"
                "}}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "walker : { include : {"
                " /foo"
                " /usr/lib/fab/builtin-goats"
                " /home/todd/my/goats"
                "}}\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                "  /usr/lib/fab/builtin-goats"
                "}}"
                "\0\0"
              }
            , (char[]) {
                "walker : { include = {"
                " /foo"
                "}}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                "  /usr/lib/fab/builtin-goats"
                "}}"
                "\0\0"
              }
            , (char[]) {
                "walker : { include : {"
                "  /foo"
                "}}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "walker : { include : {"
                " /foo"
                " /usr/lib/fab/builtin-goats"
                "}}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "walker : { include : {"
                "  /usr/lib/fab/builtin-goats"
                "}}"
                "\0\0"
              }
            , (char[]) {
                "walker : { include = {"
                "  /foo"
                "}}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "walker : { include : {"
                " /foo"
                "}}"
                "\0\0"
            }
      }}
    , 0
  }
};
