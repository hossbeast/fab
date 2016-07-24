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

#include <stdio.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "internal.h"
#include "stream.internal.h"
#include "LOGGER.errtab.h"
#include "category.internal.h"
#include "log.internal.h"

#include "test_util.h"

narrator * N;

#define L_FOO categories[0].id

static logger_category * categories = (logger_category []) {
    { name : "FOO", description : "foo" }
  , { }
};

static logger_stream * streams = (logger_stream []) {
    { name : "foo", type : LOGGER_STREAM_NARRATOR, expr : "+FOO" }
  , { }
};

static xapi test_setup()
{
  enter;

  // initialize modules
  fatal(category_setup);
  fatal(stream_setup);

  fatal(narrator_fixed_create, &N, 2048);
  streams[0].narrator = N;

  // register a stream
  fatal(logger_category_register, categories);
  fatal(logger_stream_register, streams);
  fatal(categories_activate);
  fatal(streams_activate);
  fatal(streams_finalize);

  finally : coda;
}

static void test_teardown()
{
  narrator_ifree(&N);
  stream_teardown();
  category_teardown();
  log_teardown();
}

static xapi test_log()
{
  enter;

  // act
  logs(L_FOO, "foo");

  // assert
  char * expected = "foo";
  const char * actual = narrator_fixed_buffer(N);
  assertf(strcmp(expected, actual) == 0, "%s", "%s", expected, actual);

  finally : coda;
}

static xapi test_log_start()
{
  enter;

  int token;

  // act
  fatal(log_start, L_FOO, &token);

  // only the ids specified in log_start matter
  logs(L_FOO, "f");
  logs(0, "o");
  logs(75, "o");

  fatal(log_finish, &token);

  // assert
  char * expected = "foo";
  const char * actual = narrator_fixed_buffer(N);
  assertf(strcmp(expected, actual) == 0, "%s", "%s", expected, actual);

finally:
  fatal(log_finish, &token);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  int x = 0;
  fatal(xapi_errtab_register, perrtab_LOGGER);
  fatal(xapi_errtab_register, perrtab_TEST);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_log }
    , { entry : test_log_start }
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    test_teardown();
    fatal(test_setup);

    xapi exit;
    if((exit = invoke(tests[x].entry)))
    {
      // propagate unexpected errors
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        tfail(0, 0);

      // print the stacktrace to stdout
      xapi_backtrace_to(1);
      xapi_calltree_unwind();
    }

    assert_exit(exit, perrtab_LOGGER, tests[x].expected);
    success;
  }

finally:
  test_teardown();

  if(XAPI_UNWINDING)
  {
    xapi_infof("test", "%d", x);
    xapi_backtrace();
  }
conclude(&R);
  xapi_teardown();

  return !!R;
}
