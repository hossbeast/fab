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
#include <dlfcn.h>

#define perrtab perrtab_MAIN
#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xtime.h"
#include "xlinux/xdlfcn.h"
#include "xunit.h"
#include "xunit/assert.h"
#include "xunit/XUNIT.errtab.h"
#include "logger.h"
#include "narrator.h"
#include "narrator/units.h"

#include "args.h"
#include "logging.h"
#include "errtab/MAIN.errtab.h"
#include "macros.h"

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R;
  char space[4096];
  size_t tracesz = 0;

  uint32_t suite_assertions_passed = 0;
  uint32_t suite_assertions_failed = 0;
  uint32_t suite_tests = 0;
  uint32_t suite_units = 0;

  // allocated with the same size as g_args.objects
  void ** objects = 0;
  int x = 0;
  int token = 0;

  // load libraries
  fatal(logger_load);
  fatal(xlinux_load);
  fatal(xunit_load);

  // initialize logger
  fatal(logging_setup);
  fatal(logger_arguments_setup, envp);
  fatal(logger_initialize);

  // parse cmdline arguments
  fatal(args_parse);

  // summarize
  fatal(args_summarize);

  // allocation for dloaded objects
  fatal(xmalloc, &objects, sizeof(*objects) * g_args.objectsl);

  struct timespec suite_start;
  struct timespec suite_end;
  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &suite_start);

  for(;x < g_args.objectsl; x++)
  {
    // open the object
    fatal(xdlopen, g_args.objects[x], RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND, &objects[x]);

    // load the test manifest
    xunit_unit * xunit;
    fatal(uxdlsym, objects[x], "xunit", (void*)&xunit);

    if(xunit)
    {
      logf(L_DLOAD, "loaded %s -> xunit : { setup : %p, teardown : %p, release : %p, tests : %zu }"
        , g_args.objects[x]
        , xunit->setup
        , xunit->teardown
        , xunit->release
        , sentinel(xunit->tests)
      );
    }
    else
    {
      logf(L_DLOAD, "loaded %s", g_args.objects[x]);    // not an object containing tests
    }

    if(xunit)
    {
      suite_units++;

      struct timespec unit_start;
      struct timespec unit_end;
      fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &unit_start);

      // unit setup
      if(xunit->setup)
        fatal(xunit->setup, xunit);

      xunit_test ** test = xunit->tests;

      uint32_t unit_assertions_passed = 0;
      uint32_t unit_assertions_failed = 0;
      uint32_t unit_tests = 0;

      // execute all tests
      while(*test)
      {
        unit_tests++;

        // convenience
        (*test)->unit = xunit;

        xunit_assertions_passed = 0;
        xunit_assertions_failed = 0;

        struct timespec test_start;
        struct timespec test_end;
        fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &test_start);

        if(invoke((*test)->entry, (*test)))
        {
          // add identifying info
          if((*test)->name)
            xapi_info_adds("test", (*test)->name);

          xapi_info_adds("object", g_args.objects[x]);

          // propagate non-unit-testing errors
          if(XAPI_ERRTAB != perrtab_XUNIT || XAPI_ERRCODE != XUNIT_FAIL)
            fail(0);

          // save the trace
          size_t z = xapi_trace_pithy(space, sizeof(space));

          // discard the error frames
          xapi_calltree_unwind();

          // for unit-testing errors, log the failure and continue
          logf(L_FAIL, "   %.*s", (int)z, space);
        }

        fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &test_end);

        // report for this test
        fatal(log_xstart, L_TEST, xunit_assertions_failed == 0 ? L_GREEN : L_RED, &token);
        logf(0
          , "  %%%6.2f pass rate on %6d assertions in "
          , 100 * ((double)xunit_assertions_passed / (double)(xunit_assertions_passed + xunit_assertions_failed))
          , xunit_assertions_passed + xunit_assertions_failed
        );

        fatal(elapsed_say, &test_start, &test_end, log_narrator(&token));
        fatal(log_finish, &token);

        unit_assertions_passed += xunit_assertions_passed;
        unit_assertions_failed += xunit_assertions_failed;

        test++;
      }

      fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &unit_end);

      // report for this module
      fatal(log_xstart, L_UNIT, unit_assertions_failed == 0 ? L_GREEN : L_RED, &token);
      logf(0
        , " %%%6.2f pass rate on %6d assertions by %3d tests in "
        , 100 * ((double)unit_assertions_passed / (double)(unit_assertions_passed + unit_assertions_failed))
        , unit_assertions_passed + unit_assertions_failed
        , sentinel(xunit->tests)
      );

      fatal(elapsed_say, &unit_start, &unit_end, log_narrator(&token));
      fatal(log_finish, &token);

      suite_assertions_passed += unit_assertions_passed;
      suite_assertions_failed += unit_assertions_failed;
      suite_units++;
      suite_tests += unit_tests;

      // unit cleanup
      if(xunit->teardown)
        xunit->teardown(xunit);

      if(xunit->release)
        fatal(xunit->release, xunit);
    }
  }

  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &suite_end);

  // summary report
  fatal(log_xstart, L_SUITE, suite_assertions_failed == 0 ? L_GREEN : L_RED, &token);
  logf(0
    , "%%%6.2f pass rate on %6d assertions by %3d tests from %4d units in "
    , 100 * ((double)suite_assertions_passed / (double)(suite_assertions_passed + suite_assertions_failed))
    , (suite_assertions_passed + suite_assertions_failed)
    , suite_tests
    , suite_units
  );

  fatal(elapsed_say, &suite_start, &suite_end, log_narrator(&token));
  fatal(log_finish, &token);

finally:
  fatal(log_finish, &token);

  if(XAPI_UNWINDING)
  {
# if DEVEL || DEBUG
    if(g_args.mode_backtrace == MODE_BACKTRACE_PITHY)
    {
#endif
      tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEBUG || DEVEL
    }
    else
    {
      tracesz = xapi_trace_full(space, sizeof(space));
    }
#endif

    xlogw(L_ERROR, L_RED, space, tracesz);
  }

  int j;
  for(j = 0; j < x; j++)
    fatal(xdlclose, objects[j]);
  free(objects);

  args_teardown();
  fatal(xunit_unload);
  fatal(logger_unload);

conclude(&R);
  xapi_teardown();

  return R | suite_assertions_failed;
}
