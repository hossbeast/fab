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
#include "node.h"
#include "logging.h"

#include "box.h"

typedef struct {
  XUNITTEST;

  char ** texts;
  char * expected;
} config_merge_test;

static xapi config_merge_test_unit_setup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_load);
  fatal(value_load);
  fatal(logging_finalize);
  fatal(config_setup);

  finally : coda;
}

static xapi config_merge_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(yyutil_unload);
  fatal(value_unload);
  fatal(config_cleanup);

  finally : coda;
}

static xapi config_merge_test_entry(xunit_test * _test)
{
  enter;

  config_merge_test * test = containerof(_test, config_merge_test, xu);

  config_parser * parser = 0;
  config * A = 0;
  config * B = 0;
  config * T = 0;
  narrator_growing * N1 = 0;
  narrator_growing * N2 = 0;
  char ** text = 0;

  // arrange
  fatal(config_parser_create, &parser);
  fatal(narrator_growing_create, &N1);
  fatal(narrator_growing_create, &N2);

  // act - initial parse
  text = test->texts;
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

  fatal(config_parser_parse, parser, test->expected, strlen(test->expected) + 2, 0, 0, &B);

  fatal(config_say, A, &N1->base);
  fatal(config_say, B, &N2->base);
  assert_eq_w(N2->s, N2->l, N1->s, N1->l);

finally:
  fatal(config_parser_xfree, parser);
  fatal(config_xfree, A);
  fatal(config_xfree, B);
  fatal(config_xfree, T);
  fatal(narrator_growing_free, N1);
  fatal(narrator_growing_free, N2);
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

      /* extern */
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "extern : {"
                "  /usr/lib/fab/builtin-modules"
                "  /home/todd/my/modules"
                "}\0\0"
              }
            , (char[]) {
                "extern :{"
                "  foo"
                "}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "extern : {"
                " foo"
                " /usr/lib/fab/builtin-modules"
                " /home/todd/my/modules"
                "}\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "extern : {"
                "  /usr/lib/fab/builtin-modules"
                "}"
                "\0\0"
              }
            , (char[]) {
                "extern = {"
                " foo"
                "}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "extern : {"
                "  /usr/lib/fab/builtin-modules"
                "}"
                "logging : {"
                " console : {"
                "  exprs : ["
                "   foo"
                "  ]"
                " }"
                "}"
                "\0\0"
              }
            , (char[]) {
                "extern : {"
                "  foo"
                "}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "extern : {"
                " /usr/lib/fab/builtin-modules"
                " foo"
                "}"
                "logging : {"
                " console : {"
                "  exprs : ["
                "   foo"
                "  ]"
                " }"
                "}"
                "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "extern : {"
                "  /usr/lib/fab/builtin-modules"
                "}"
                "logging : {"
                " console : {"
                "  exprs : ["
                "   foo"
                "  ]"
                " }"
                "}"
                "\0\0"
              }
            , (char[]) {
                "extern = {"
                "  foo"
                "}\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "extern : {"
                " foo"
                "}"
                "logging : {"
                " console : {"
                "  exprs : ["
                "   foo"
                "  ]"
                " }"
                "}"
                "\0\0"
            }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "logging : {"
                " console : {"
                "  exprs : ["
                "   foo"
                "  ]"
                " }"
                "}"
                "\0\0"
              }
            , (char[]) {
                "logging : {"
                " console : {"
                "  exprs : ["
                "   bar"
                "  ]"
                " }"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "logging : {"
                " console : {"
                "  exprs : ["
                "   foo"
                "   bar"
                "  ]"
                " }"
                "}"
                "\0\0"
            }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "extern : {"
                " foo"
                "}"
                "\0\0"
              }
            , (char[]) {
                "logging : {"
                " console : {"
                "  exprs : ["
                "   bar"
                "  ]"
                " }"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "extern : {"
                " foo"
                "}"
                "logging : {"
                " console : {"
                "  exprs : ["
                "   bar"
                "  ]"
                " }"
                "}"
                "\0\0"
            }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "logging : {"
                " console : {"
                "  exprs : ["
                "   bar"
                "  ]"
                " }"
                "}"
                "\0\0"
              }
            , (char[]) {
                "logging = {}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
                "\0\0"
            }
      }}
    /* var */
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "var : {"
                " cflags : -D"
                "}"
                "\0\0"
              }
            , (char[]) {
                "var : {"
                " lflags : -D"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
            "var : {"
            " cflags : -D"
            " lflags : -D"
            "}"
            "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "var : {"
                " cflags : -D"
                "}"
                "\0\0"
              }
            , (char[]) {
                "var = {"
                " lflags.xapi = -D"
                "}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
            "var : {"
            " lflags.xapi : -D"
            "}"
            "\0\0"
          }
      }}
    , (config_merge_test[]) {{
          texts : (char*[]) {
              (char[]) {
                "var : {"
                " cflags : -D"
                " xflags : -D"
                " flags : -D"
                "}"
                "\0\0"
              }
            , (char[]) {
                "var = {}"
                "\0\0"
              }
            , 0
          }
        , expected : (char[]) {
            "var : {}"
            "\0\0"
          }
      }}
    , 0
  }
};
