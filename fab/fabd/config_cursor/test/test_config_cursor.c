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
  assert_eq_s("foo.bar", config_cursor_path(&cursor));
  assert_eq_s("foo.bar", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushs, &cursor, "baz.qux");
  assert_eq_s("foo.bar.baz.qux", config_cursor_path(&cursor));
  assert_eq_s("baz.qux", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushd, &cursor, 7);
  assert_eq_s("foo.bar.baz.qux.7", config_cursor_path(&cursor));
  assert_eq_s("7", config_cursor_query(&cursor));

  fatal(config_cursor_sets, &cursor, "valyria");
  assert_eq_s("valyria", config_cursor_path(&cursor));
  assert_eq_s("valyria", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushf, &cursor, "%s", "winterfell");
  assert_eq_s("valyria.winterfell", config_cursor_path(&cursor));
  assert_eq_s("winterfell", config_cursor_query(&cursor));

finally:
  config_cursor_destroy(&cursor);
coda;
}
#include "valyria/pstring.h"
static xapi config_cursor_push_repeated(xunit_test * test)
{
  enter;

  config_cursor cursor;
  fatal(config_cursor_init, &cursor);

  fatal(config_cursor_sets, &cursor, "foo.bar");
  assert_eq_s("foo.bar", config_cursor_path(&cursor));
  assert_eq_s("foo.bar", config_cursor_query(&cursor));

  fatal(config_cursor_mark, &cursor);
  fatal(config_cursor_pushs, &cursor, "baz.qux");
  assert_eq_s("foo.bar.baz.qux", config_cursor_path(&cursor));
  assert_eq_s("baz.qux", config_cursor_query(&cursor));

  fatal(config_cursor_pushd, &cursor, 7);

  assert_eq_s("foo.bar.7", config_cursor_path(&cursor));
  assert_eq_s("7", config_cursor_query(&cursor));

  fatal(config_cursor_pushd, &cursor, 33);
  assert_eq_s("foo.bar.33", config_cursor_path(&cursor));
  assert_eq_s("33", config_cursor_query(&cursor));

finally:
  config_cursor_destroy(&cursor);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_tests : (xunit_test*[]) {
      (xunit_test[]) {{ xu_entry : config_cursor_basic }}
    , (xunit_test[]) {{ xu_entry : config_cursor_push_repeated }}
    , 0
  }
};
