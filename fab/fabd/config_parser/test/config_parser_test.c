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
#include "xlinux/xstdlib.h"

#include "xlinux.h"
#include "valyria.h"
#include "fabcore.h"
#include "lorien.h"
#include "yyutil.h"

struct config_parser_test;
#define TEST_TYPE struct config_parser_test
#include "xunit.h"
#include "xunit/assert.h"

#include "value.h"
#include "value/value_store.h"
#include "logger.h"
#include "logger/category.h"

#include "config_parser.h"
#include "config_parser_test.h"

#include "snarf.h"
#include "macros.h"

//
// public
//

xapi config_parser_unit_setup(xunit_unit * unit)
{
  enter;

  int x;
  for(x = 0; x < sentinel(unit->tests); x++)
    unit->tests[x]->entry = config_parser_test_entry;

  // load libraries
  fatal(lorien_load);
  fatal(fabcore_load);
  fatal(yyutil_load);
  fatal(value_load);

  // logging
  fatal(logger_initialize);

  finally : coda;
}

xapi config_parser_unit_release(xunit_unit * unit)
{
  enter;

  fatal(lorien_unload);
  fatal(fabcore_unload);
  fatal(yyutil_unload);
  fatal(value_unload);

  finally : coda;
}

xapi config_parser_test_entry(config_parser_test * test)
{
  enter;

  config_parser * parser = 0;
  value_store * store = 0;
  char * text = 0;
  size_t len;

  fatal(config_parser_create, &parser);

  // load the config text
  fatal(snarf, test->config_path, &text, &len);

  // parse
  value * root = 0;
  fatal(config_parser_parse, &parser, &store, text, len, &root);

finally:
  config_parser_free(parser);
  value_store_free(store);
  xfree(text);
coda;
}
