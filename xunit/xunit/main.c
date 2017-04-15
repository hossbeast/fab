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
#include <inttypes.h>

#include "xapi/SYS.errtab.h"

#include "xapi.h"

#include "xlinux/load.h"
#include "logger/load.h"
#include "xunit/load.h"
#include "narrator/load.h"

#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xtime.h"
#include "xlinux/xdlfcn.h"
#include "xunit/assert.h"
#include "xunit/XUNIT.errtab.h"
#include "logger.h"
#include "narrator.h"
#include "narrator/units.h"

#include "args.h"
#include "logging.h"
#include "MAIN.errtab.h"
#include "macros.h"

static uint32_t suite_assertions_passed = 0;
static uint32_t suite_assertions_failed = 0;
static uint32_t suite_tests_passed = 0;
static uint32_t suite_tests_failed = 0;
static uint32_t suite_units = 0;

static xapi begin(int argc, char** argv, char ** envp)
{
  enter;

  char trace[4096];
  Dl_info nfo;
  narrator * N;

  // allocated with the same size as g_args.objects
  void ** objects = 0;
  int x = 0;

  // parse cmdline arguments
  fatal(args_parse);
  fatal(args_summarize);

  // allocation for dloaded objects
  fatal(xmalloc, &objects, sizeof(*objects) * g_args.objectsl);

  struct timespec suite_start;
  struct timespec suite_end;
  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &suite_start);

  for(x = 0; x < g_args.objectsl; x++)
  {
    /*
     * When the object is unloaded, its symbols are no longer available. It's important
     * that the symbols remain available, for several reasons:
     *  1) valgrind leak reports will have blank frames for functions in those symbols
     *  2) xapi/trace will report an unknown error, if the error table is one of those symbols
     */
    fatal(xdlopen
      , g_args.objects[x]
      , RTLD_NOW          // fail before dlopen returns if the object has unresolved references
      | RTLD_GLOBAL       // make symbols in this object available to subsequently loaded objects
      | RTLD_DEEPBIND     // cause this object to prefer symbols which it contains
      | RTLD_NODELETE     // dont unload during dlclose
      , &objects[x]
    );

    // load the test manifest
    xunit_unit * xunit;
    fatal(uxdlsym, objects[x], "xunit", (void*)&xunit);

    if(xunit)
    {
      logf(L_DLOAD, "loaded %s -> xunit : { setup : %p, teardown : %p, cleanup : %p, tests : %zu }"
        , g_args.objects[x]
        , xunit->xu_setup
        , xunit->xu_teardown
        , xunit->xu_cleanup
        , sentinel(xunit->xu_tests)
      );
    }
    else
    {
      logf(L_DLOAD, "loaded %s", g_args.objects[x]);    // not an object containing tests
    }

    if(xunit)
    {
      struct timespec unit_start;
      struct timespec unit_end;
      fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &unit_start);

      // unit setup
      if(xunit->xu_setup)
        fatal(xunit->xu_setup, xunit);

      xunit_test ** test = xunit->xu_tests;

      uint32_t unit_tests_passed = 0;
      uint32_t unit_tests_failed = 0;
      uint32_t unit_assertions_passed = 0;
      uint32_t unit_assertions_failed = 0;

      int only = 0;
      while(*test)
      {
        // convenience
        (*test)->xu_unit = xunit;

        if((*test)->xu_only)
          only++;

        test++;
      }
      size_t tests_len = (test - xunit->xu_tests) - 1;

      // execute all tests
      test = xunit->xu_tests;
      for(test = xunit->xu_tests; *test; test++)
      {
        if(only && !(*test)->xu_only)
          continue;

        // convenience
        (*test)->xu_unit = xunit;

        xunit_test_entry entry = (*test)->xu_entry ?: xunit->xu_entry;

        xunit_assertions_passed = 0;
        xunit_assertions_failed = 0;

        struct timespec test_start;
        struct timespec test_end;
        fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &test_start);

        const char * name = 0;
        if(!(name = (*test)->xu_name))
        {
          int r = dladdr(entry, &nfo);
          if(r && nfo.dli_sname)
            name = nfo.dli_sname;
        }

        int test_failed = 0;
        if(invoke(entry, *test))
        {
          test_failed = 1;

          // add identifying info
          if(name)
            xapi_frame_info_pushs("name", name);
          xapi_frame_info_pushf("test", "%zu in [%d,%zu]", test - xunit->xu_tests, 0, tests_len);

          // propagate non-unit-testing errors
          if(XAPI_ERRVAL != XUNIT_FAIL)
            fail(0);

          // for unit-testing errors, log the failure and continue
          xapi_trace_full(trace, sizeof(trace), XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
          xapi_calltree_unwind();
          logs(L_FAIL, trace);
        }
        else if(xunit_assertions_failed)
        {
          test_failed = 1;
        }

        fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &test_end);

        // report for this test
        fatal(log_xstart, L_TEST, test_failed ? L_RED : L_GREEN, &N);
        sayf("  %%%6.2f pass rate on %6"PRIu32" assertions over "
          , 100 * ((double)xunit_assertions_passed / (double)(xunit_assertions_passed + xunit_assertions_failed))
          , xunit_assertions_passed + xunit_assertions_failed
        );
        fatal(elapsed_say, &test_start, &test_end, N);
        if(name)
          sayf(" for %s", name);

        fatal(log_finish);

        if(test_failed)
          unit_tests_failed++;
        else
          unit_tests_passed++;
        unit_assertions_passed += xunit_assertions_passed;
        unit_assertions_failed += xunit_assertions_failed;
      }

      fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &unit_end);

      // report for this module
      fatal(log_xstart, L_UNIT, unit_tests_failed ? L_RED : L_GREEN, &N);
      sayf(" %%%6.2f pass rate on %6"PRIu32" assertions by %3"PRIu32" tests over "
        , 100 * ((double)unit_assertions_passed / (double)(unit_assertions_passed + unit_assertions_failed))
        , unit_assertions_passed + unit_assertions_failed
        , unit_tests_passed + unit_tests_failed
      );

      fatal(elapsed_say, &unit_start, &unit_end, N);
      sayf(" in %s", g_args.objects[x]);
      fatal(log_finish);

      suite_assertions_passed += unit_assertions_passed;
      suite_assertions_failed += unit_assertions_failed;
      suite_tests_passed += unit_tests_passed;
      suite_tests_failed += unit_tests_failed;
      suite_units++;

      // unit cleanup
      if(xunit->xu_teardown)
        xunit->xu_teardown(xunit);

      if(xunit->xu_cleanup)
        fatal(xunit->xu_cleanup, xunit);
    }
  }

  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &suite_end);

  // summary report
  fatal(log_xstart, L_SUITE, suite_tests_failed ? L_RED : L_GREEN, &N);
  sayf("%%%6.2f pass rate on %6d assertions by %3d tests from %4d units over "
    , 100 * ((double)suite_assertions_passed / (double)(suite_assertions_passed + suite_assertions_failed))
    , (suite_assertions_passed + suite_assertions_failed)
    , suite_tests_passed + suite_tests_failed
    , suite_units
  );

  fatal(elapsed_say, &suite_start, &suite_end, N);
  fatal(log_finish);

finally:
  if(objects)
  {
    for(x = 0; x < g_args.objectsl; x++)
      fatal(ixdlclose, &objects[x]);
  }

  // locals
  wfree(objects);
coda;
}

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R;

  // load libraries
  fatal(logger_load);
  fatal(narrator_load);
  fatal(xlinux_load);
  fatal(xunit_load);

  // initialize logger
  fatal(logging_setup, envp);

  // main program
  fatal(begin, argc, argv, envp);

finally:
  if(XAPI_UNWINDING)
    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE);

  // modules
  args_teardown();

  // libraries
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(xlinux_unload);
  fatal(xunit_unload);

conclude(&R);
  xapi_teardown();

  R |= suite_assertions_failed;
  R |= suite_tests_failed;

  return !!R;
}
