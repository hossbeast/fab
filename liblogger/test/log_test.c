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

#include "test_util.h"

narrator * N;

/// test_setup
//
// SUMMARY
//  initialize the module
//
xapi test_setup()
{
  enter;

  // initialize 
  fatal(category_setup);
  fatal(stream_setup);

  fatal(narrator_fixed_create, &N, 2048);

  // register a stream
  logger_stream * streams = (logger_stream []) {
      { name : "foo", type : LOGGER_STREAM_NARRATOR, narrator : N }
    , { }
  };

  fatal(logger_stream_register, streams);
  fatal(stream_activate);

  finally : coda;
}

void test_teardown()
{
  narrator_ifree(&N);
  stream_teardown();
  category_teardown();
}

/// test_log_zero
//
// categories of zero should always be emitted
//
xapi test_log_zero()
{
  enter;

  // act
  logs(0, "foo");

  // assert
  char * expected = "foo";
  const char * actual = narrator_fixed_buffer(N);
  assertf(strcmp(expected, actual) == 0, "%s", "%s", expected, actual);

  finally : coda;
}

xapi test_log_start()
{
  enter;

  int token;

  // act
  fatal(log_start, 0, &token);

  // only the ids specified in log_start matter
  logs(0xa, "f");
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

  int x = 0;
  fatal(xapi_errtab_register, perrtab_LOGGER);
  fatal(xapi_errtab_register, perrtab_TEST);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_log_zero }
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
  }

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_infof("test", "%d", x);
    xapi_backtrace();
  }

  test_teardown();
coda;
}
