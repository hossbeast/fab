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
#include <string.h>
#include <sys/syscall.h>

#include "xapi.h"
#include "xapi/errtab.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "logger.h"
#include "stream.internal.h"
#include "LOGGER.errtab.h"
#include "arguments.internal.h"
#include "category.internal.h"
#include "log.internal.h"

#include "test_util.h"
#include "macros.h"

__thread int32_t tid;

static logger_category * logs_test_one = (logger_category[]) {
    { name : "ABC" }
  , { name : "BAR" }
  , { name : "DELTA" }
  , {}
};

static logger_category * logs_test_two = (logger_category[]) {
    { name : "BAZ" }
  , {}
};

static void test_teardown()
{
  arguments_teardown();
}

static xapi test_arguments_nonlogger()
{
  enter;

  // arrange
  char * argvs = "foo\0bar\0baz\0";
  size_t argvsl = 12;

  // act
  fatal(arguments_process, argvs, argvsl, -1, -1);
  fatal(arguments_finalize);

  // assert
  assert_eq_d(3, g_argc);
  assert_eq_d(0, g_logc);
  assert_eq_d(0, g_ulogc);
  assert_eq_s("foo", g_argv[0]);
  assert_eq_s("bar", g_argv[1]);
  assert_eq_s("baz", g_argv[2]);
  assert_eq_d(11, g_argvsl);
  assert_eq_d(0, g_logvsl);
  assert_eq_d(0, g_ulogvsl);
  assert_eq_s("foo bar baz", g_argvs);

  finally : coda;
}

static xapi test_arguments_recognized()
{
  enter;

  // arrange
  char * argvs = "+ABC\0+BAR\0";
  size_t argvsl = 10;

  // act
  fatal(arguments_process, argvs, argvsl, -1, -1);
  fatal(arguments_finalize);

  // assert
  assert_eq_d(0, g_argc);
  assert_eq_d(2, g_logc);
  assert_eq_d(0, g_ulogc);
  assert_eq_s("+ABC", g_logv[0]);
  assert_eq_s("+BAR", g_logv[1]);
  assert_eq_d(0, g_argvsl);
  assert_eq_d(9, g_logvsl);
  assert_eq_d(0, g_ulogvsl);
  assert_eq_s("+ABC +BAR", g_logvs);

  finally : coda;
}

static xapi test_arguments_unrecognized()
{
  enter;

  // arrange
  char * argvs = "+BAZ\0+QUX\0";
  size_t argvsl = 10;

  // act
  fatal(arguments_process, argvs, argvsl, -1, -1);
  fatal(arguments_finalize);

  // assert
  assert_eq_d(0, g_argc);
  assert_eq_d(0, g_logc);
  assert_eq_d(2, g_ulogc);
  assert_eq_s("+BAZ", g_ulogv[0]);
  assert_eq_s("+QUX", g_ulogv[1]);
  assert_eq_d(0, g_argvsl);
  assert_eq_d(0, g_logvsl);
  assert_eq_d(9, g_ulogvsl);
  assert_eq_s("+BAZ +QUX", g_ulogvs);

  finally : coda;
}

static xapi test_arguments_mixed()
{
  enter;

  // arrange
  char * argvs = "+BAZ\0+QUX\0+ABC\0foo\0bar\0";
  size_t argvsl = 23;

  // act
  fatal(arguments_process, argvs, argvsl, -1, -1);
  fatal(arguments_finalize);

  // assert
  assert_eq_d(2, g_argc);
  assert_eq_d(1, g_logc);
  assert_eq_d(2, g_ulogc);
  assert_eq_s("foo", g_argv[0]);
  assert_eq_s("bar", g_argv[1]);
  assert_eq_s("+ABC", g_logv[0]);
  assert_eq_s("+BAZ", g_ulogv[0]);
  assert_eq_s("+QUX", g_ulogv[1]);
  assert_eq_d(7, g_argvsl);
  assert_eq_d(4, g_logvsl);
  assert_eq_d(9, g_ulogvsl);
  assert_eq_s("foo bar", g_argvs);
  assert_eq_s("+ABC", g_logvs);
  assert_eq_s("+BAZ +QUX", g_ulogvs);

  finally : coda;
}

static xapi test_arguments_repeated()
{
  enter;

  // arrange
  char * argvs = "+BAZ\0+QUX\0+ABC\0foo\0bar\0";
  size_t argvsl = 23;

  // act
  fatal(arguments_process, argvs, argvsl, -1, -1);
  fatal(arguments_finalize);

  // assert
  assert_eq_d(2, g_argc);
  assert_eq_d(1, g_logc);
  assert_eq_d(2, g_ulogc);
  assert_eq_s("foo", g_argv[0]);
  assert_eq_s("bar", g_argv[1]);
  assert_eq_s("+ABC", g_logv[0]);
  assert_eq_s("+BAZ", g_ulogv[0]);
  assert_eq_s("+QUX", g_ulogv[1]);

  // arrange
  fatal(logger_category_register, logs_test_two);
  fatal(categories_activate);

  // act
  fatal(arguments_finalize);

  // assert
  assert_eq_d(2, g_argc);
  assert_eq_d(2, g_logc);
  assert_eq_d(1, g_ulogc);
  assert_eq_s("foo", g_argv[0]);
  assert_eq_s("bar", g_argv[1]);
  assert_eq_s("+ABC", g_logv[0]);
  assert_eq_s("+BAZ", g_logv[1]);
  assert_eq_s("+QUX", g_ulogv[1]);

  finally : coda;
}

static xapi run_tests()
{
  enter;

  int x = 0;

  fatal(category_setup);
  fatal(stream_setup);
  fatal(logger_category_register, logs_test_one);
  fatal(categories_activate);

  struct {
    xapi (*entry)();
    int expected;
  } tests[] = {
      { entry : test_arguments_nonlogger }
    , { entry : test_arguments_recognized }
    , { entry : test_arguments_unrecognized }
    , { entry : test_arguments_mixed }
    , { entry : test_arguments_repeated }
  };

  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    test_teardown();

    xapi exit;
    if((exit = invoke(tests[x].entry)))
    {
      // propagate unexpected errors
      if(xapi_exit_errtab(exit) != perrtab_LOGGER)
        fail(0);

      // print the stacktrace to stdout
      xapi_backtrace(1, 0);
      xapi_calltree_unwind();
    }

    assert_eq_exit(tests[x].expected, exit);
  }

finally:
  fatal(category_cleanup);
  fatal(stream_cleanup);
  test_teardown();

  summarize;
coda;
}

int main()
{
  enter;

  tid = syscall(SYS_gettid);
  xapi R = 0;
  fatal(run_tests);

finally:
  if(XAPI_UNWINDING)
    xapi_backtrace(2, 0);
conclude(&R);
  xapi_teardown();

  return !!R;
}
