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
#include "xapi/errtab.h"
#include "xlinux/xstdlib.h"

#include "xlinux/load.h"
#include "valyria/load.h"
#include "lorien/load.h"
#include "yyutil/load.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "value.h"
#include "value/store.h"
#include "value/make.h"
#include "logger.h"
#include "logger/category.h"

#include "config_parser.h"
#include "errtab/CONFIG.errtab.h"

#include "macros.h"

//
// public
//

static xapi config_parser_unit_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(lorien_load);
  fatal(yyutil_load);
  fatal(value_load);

  // modules
  fatal(xapi_errtab_register, perrtab_CONFIG);

  // logging
  fatal(logger_finalize);

  finally : coda;
}

static xapi config_parser_unit_cleanup(xunit_unit * unit)
{
  enter;

  fatal(lorien_unload);
  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

//
// tests
//

static xapi config_parser_test_maps(xunit_test * test)
{
  enter;

  config_parser * parser = 0;
  value_store * store = 0;
  value * actual = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(narrator_fixed_create, &N0, 1024);
  fatal(narrator_fixed_create, &N1, 1024);

  char * config_text = "core.filesystem { \"/\" { invalidate notify } \"/mnt/remote\" { invalidate stat } }";

  // parse
  fatal(config_parser_create, &parser);
  fatal(config_parser_parse, &parser, &store, MMS(config_text), &actual);

  // assert
  value * val = 0;
  value * map = 0;
  value * map2 = 0;
  value * expected = 0;

  fatal(value_string_mks, store, 0, 0, &val, "notify");
  fatal(value_map_mks, store, 0, 0, &map, "invalidate", val, 0);
  fatal(value_map_mks, store, 0, map2, &map2, "/", map, 0);
  fatal(value_string_mks, store, 0, 0, &val, "stat");
  fatal(value_map_mks, store, 0, 0, &map, "invalidate", val, 0);
  fatal(value_map_mks, store, 0, map2, &map2, "/mnt/remote", map, 0);
  fatal(value_map_mks, store, 0, 0, &map, "filesystem", map2, 0);
  fatal(value_map_mks, store, 0, 0, &expected, "core", map, 0);
  
  fatal(value_say, expected, N0);
  fatal(value_say, actual, N1);
  assertf(value_cmp(expected, actual) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

finally:
  fatal(config_parser_xfree, parser);
  fatal(value_store_xfree, store);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
coda;
}

static xapi config_parser_test_lists(xunit_test * test)
{
  enter;

  config_parser * parser = 0;
  value_store * store = 0;
  value * actual = 0;
  narrator * N0 = 0;
  narrator * N1 = 0;

  fatal(narrator_fixed_create, &N0, 1024);
  fatal(narrator_fixed_create, &N1, 1024);

  char * config_text = "core [ 1 true 2.0 \"foo\" ]";

  // parse
  fatal(config_parser_create, &parser);
  fatal(config_parser_parse, &parser, &store, MMS(config_text), &actual);

  // assert
  value * val = 0;
  value * list = 0;
  value * expected = 0;

  fatal(value_integer_mk, store, 0, &val, 1);
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_boolean_mk, store, 0, &val, 1);
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_float_mk, store, 0, &val, 2);
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_string_mks, store, 0, 0, &val, "foo");
  fatal(value_list_mkv, store, 0, list, &list, val);
  fatal(value_map_mks, store, 0, 0, &expected, "core", list, 0);
  
  fatal(value_say, expected, N0);
  fatal(value_say, actual, N1);
  assertf(value_cmp(expected, actual) == 0, "%s", "%s", narrator_fixed_buffer(N0), narrator_fixed_buffer(N1));

finally:
  fatal(config_parser_xfree, parser);
  fatal(value_store_xfree, store);
  fatal(narrator_xfree, N0);
  fatal(narrator_xfree, N1);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    setup : config_parser_unit_setup
  , cleanup : config_parser_unit_cleanup
  , tests : (xunit_test*[]) {
      (xunit_test[]) {{ entry : config_parser_test_maps }}
    , (xunit_test[]) {{ entry : config_parser_test_lists }}
    , 0
  }
};
