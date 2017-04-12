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

#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "narrator.h"
#include "narrator/load.h"
#include "narrator/growing.h"

#include "internal.h"
#include "stream.internal.h"
#include "LOGGER.errtab.h"
#include "category.internal.h"
#include "log.internal.h"

#include "test_util.h"
#include "macros.h"

narrator * N;

#define L_FOO categories[0].id

static logger_category * categories = (logger_category []) {
    { name : "FOO", description : "foo" }
  , { }
};

static logger_stream * streams = (logger_stream []) {
    { name : "foo", type : LOGGER_STREAM_NARRATOR, expr : "+FOO", exprs : (char*[]) { "+FOO", 0 } }
  , { }
};

static xapi test_setup()
{
  enter;

  // initialize modules
  fatal(category_setup);
  fatal(stream_setup);

  fatal(narrator_growing_create, &N);
  streams[0].narrator = N;

  // register a stream
  fatal(logger_category_register, categories);
  fatal(logger_stream_register, streams);
  fatal(categories_activate);
  fatal(streams_activate);
  fatal(streams_finalize);

  finally : coda;
}

static xapi test_cleanup()
{
  enter;

  fatal(narrator_ixfree, &N);
  fatal(stream_cleanup);
  fatal(category_cleanup);
  fatal(log_cleanup);

  finally : coda;
}

static xapi test_log()
{
  enter;

  char buf[64] = {};

  // act
  logs(L_FOO, "foo");

  narrator_growing_reset(N);
  narrator_growing_read(N, buf, sizeof(buf));

  // assert
  const char * expected = "foo";
  const char * actual = buf;
  assert_eq_s(expected, actual);

  finally : coda;
}

static xapi test_log_start_basic()
{
  enter;

  char buf[64] = {};

  // act
  narrator * N;
  fatal(log_start, L_FOO, &N);

  says("f");
  says("o");
  says("o");

  fatal(log_finish);

  narrator_growing_reset(N);
  narrator_growing_read(N, buf, sizeof(buf));

  // assert
  const char * expected = "foo";
  const char * actual = buf;
  assert_eq_s(expected, actual);

  finally : coda;
}

static xapi test_log_start_nullity()
{
  enter;

  char buf[64] = {};

  // act
  narrator * N;
  fatal(log_start, 42, &N); // returns the nullity narrator

  says("f");
  says("o");
  says("o");

  fatal(log_finish);

  narrator_growing_reset(N);
  narrator_growing_read(N, buf, sizeof(buf));

  // assert
  const char * expected = "";
  const char * actual = buf;
  assert_eq_s(expected, actual);

  finally : coda;
}

int main()
{
  enter;

  xapi R = 0;
  int x = 0;

  fatal(narrator_load);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_log }
    , { entry : test_log_start_basic }
    , { entry : test_log_start_nullity }
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(test_cleanup);
    fatal(test_setup);

    xapi exit;
    if((exit = invoke(tests[x].entry)))
    {
      // propagate unexpected errors
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        fail(0);

      // print the stacktrace to stdout
      xapi_backtrace_to(1);
      xapi_calltree_unwind();
    }

    assert_eq_exit(tests[x].expected, exit);
  }

finally:
  summarize;
  fatal(test_cleanup);

  if(XAPI_UNWINDING)
    xapi_backtrace();

  fatal(narrator_unload);

conclude(&R);
  xapi_teardown();

  return !!R;
}
