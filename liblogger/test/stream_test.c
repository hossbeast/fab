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

#include "internal.h"
#include "stream.internal.h"
#include "LOGGER.errtab.h"

#include "test_util.h"

/// test_setup
//
// SUMMARY
//  initialize the module
//
xapi test_setup()
{
  enter;

  stream_teardown();
  fatal(stream_setup);

  finally : coda;
}

xapi test_stream_would()
{
  enter;

  logger_stream * streams = (logger_stream []) {
      { name : "foo", type : LOGGER_STREAM_FD, .fd = 1 }
    , { }
  };

  fatal(logger_stream_register, streams);

  finally : coda;
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
      { entry : test_stream_would }
/*
    , { entry : test_category_list_merge_failure_order_single, expected : LOGGER_ILLORDER }
*/
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
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
  if(XAPI_UNWINDING)
  {
    xapi_infof("test", "%d", x);
    xapi_backtrace();
  }
coda;
}
