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

#include "xunit.h"
#include "xunit/assert.h"

#include "config_cursor.h"

//
// tests
//

static xapi config_cursor_basic(xunit_test * test)
{
  enter;

  config_cursor cursor;
  fatal(config_cursor_init, &cursor);

  fatal(config_cursor_sets, &cursor, "foo.bar");
  assert_strs("foo.bar", config_cursor_path(&cursor));
  assert_strs("foo.bar", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushs, &cursor, "baz.qux");
  assert_strs("foo.bar.baz.qux", config_cursor_path(&cursor));
  assert_strs("baz.qux", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushd, &cursor, 7);
  assert_strs("foo.bar.baz.qux.7", config_cursor_path(&cursor));
  assert_strs("7", config_cursor_query(&cursor));

  fatal(config_cursor_sets, &cursor, "valyria");
  assert_strs("valyria", config_cursor_path(&cursor));
  assert_strs("valyria", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushf, &cursor, "%s", "winterfell");
  assert_strs("valyria.winterfell", config_cursor_path(&cursor));
  assert_strs("winterfell", config_cursor_query(&cursor));

finally:
  config_cursor_destroy(&cursor);
coda;
}


//
// manifest
//

xunit_unit xunit = {
    tests : (xunit_test*[]) {
      (xunit_test[]) {{ entry : config_cursor_basic }}
    , 0
  }
};
