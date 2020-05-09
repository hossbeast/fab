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

#include "valyria/dictionary.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"

#include "filesystem.internal.h"
#include "config_parser.h"
#include "config.internal.h"
#include "logging.h"

typedef struct {
  XUNITTEST;

  char * config;
  char * filesystems;
} filesystem_test;

static xapi filesystem_test_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(lorien_load);
  fatal(yyutil_load);
  fatal(value_load);

  fatal(logging_finalize);
  fatal(filesystem_setup);

  finally : coda;
}

static xapi filesystem_test_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(lorien_unload);
  fatal(yyutil_unload);
  fatal(value_unload);

  fatal(filesystem_cleanup);

  finally : coda;
}

static xapi filesystem_test_entry(xunit_test * _test)
{
  enter;

  filesystem_test * test = (filesystem_test *)_test;

  config_parser * parser = 0;
  config * cfg = 0;
  char space[1024];

  // arrange
  fatal(config_parser_create, &parser);
  fatal(config_parser_parse, parser, test->config, strlen(test->config) + 2, 0, 0, &cfg);

  cfg->filesystems.changed = true;

  // act
  fatal(filesystem_cleanup);
  fatal(filesystem_setup);

  fatal(filesystem_reconfigure, cfg, true);
  fatal(filesystem_reconfigure, cfg, false);

  fstree_znload(space, sizeof(space));
  assert_eq_s(test->filesystems, space);

finally:
  fatal(config_parser_xfree, parser);
  fatal(config_xfree, cfg);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : filesystem_test_unit_setup
  , xu_cleanup : filesystem_test_unit_cleanup
  , xu_entry : filesystem_test_entry
  , xu_tests : (void*)(filesystem_test*[]) {
      (filesystem_test[]) {{
          config : (char[]) { "\0\0" }
        , filesystems :
              "/ : (none)"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
              "\"/B\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n B : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
              "\"/A/B\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n  B : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/\" : { invalidate : content } "
              "\"/A\" : { invalidate : notify } "
              "\"/A/B/C\" : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : content"
            "\n A : notify"
            "\n  B"
            "\n   C : stat"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\" : { invalidate : notify } "
            "}\0\0"
          }
        , filesystems :
              "/ : (none)"
            "\n A : notify"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\" : { invalidate : notify } "
              "\"/B\" : { invalidate : content } "
            "}\0\0"
          }
        , filesystems :
              "/ : (none)"
            "\n A : notify"
            "\n B : content"
            "\n"
      }}
    , (filesystem_test[]) {{
          config : (char[]) {
            "filesystems : {"
              "\"/A\"      : { invalidate : content } "
              "\"/A/BB/C\" : { invalidate : notify } "
              "\"/A/B\"    : { invalidate : stat } "
            "}\0\0"
          }
        , filesystems :
              "/ : (none)"
            "\n A : content"
            "\n  B : stat"
            "\n  BB"
            "\n   C : notify"
            "\n"
      }}
    , 0
  }
};
