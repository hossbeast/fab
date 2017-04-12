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
#include "value/load.h"
#include "yyutil/load.h"

#include "value.h"
#include "value/store.h"
#include "valyria/map.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"

#include "filesystem.internal.h"
#include "reconfigure.h"
#include "config_parser.h"
#include "logging.h"

typedef struct
{
  xunit_test;

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

  reconfigure_context ctx;
  value_store * stor = 0;
  value * config = 0;

  // modules
  fatal(filesystem_cleanup);
  fatal(filesystem_setup);

  // arrange
  fatal(config_parser_parse, 0, &stor, MMS(test->config), 0, &config);

  // act
  fatal(filesystem_reconfigure, &ctx, config, ~0);
  fatal(filesystem_reconfigure, &ctx, config, 0);

  // assert
  const char * def = test->filesystems;
  while(*def)
  {
    const char * end = def;
    while(*end && *end != ' ')
      end++;

    int leaf = 0;
    if(def[0] == '!')
    {
      leaf = 1;
      def++;
    }

    filesystem * fs = map_get(filesystems, def, end - def);

    assert_infof("name", "%.*s", (int)(end - def), def);
    assert_notnull(fs);
    assert_eq_b(leaf, fs->leaf);
    assert_info_unstage();

    def = end;
    while(*def == ' ')
      def++;
  }

finally:
  fatal(value_store_xfree, stor);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : filesystem_test_unit_setup
  , xu_cleanup : filesystem_test_unit_cleanup
  , xu_entry : filesystem_test_entry
  , xu_tests : (xunit_test*[]) {
      (filesystem_test[]) {{
          config : ""
        , filesystems : "!/"
      }}
    , (filesystem_test[]) {{
          config : "filesystem {"
              "\"/\"      { invalidate notify } "
            "}"
        , filesystems : "!/"
      }}
    , (filesystem_test[]) {{
          config : "filesystem {"
              "\"/A\"     { invalidate notify } "
            "}"
        , filesystems : "/ !/A"
      }}
    , (filesystem_test[]) {{
          config : "filesystem {"
              "\"/A\"     { invalidate notify } "
              "\"/B\"     { invalidate notify } "
            "}"
        , filesystems : "/ !/A !/B"
      }}
    , (filesystem_test[]) {{
          config : "filesystem {"
              "\"/A\"      { invalidate notify } "
              "\"/A/BB/C\" { invalidate notify } "
              "\"/A/B\"    { invalidate notify } "
            "}"
        , filesystems : "/ /A !/A/B !/A/BB/C"
      }}
    , 0
  }
};
