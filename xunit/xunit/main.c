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
#include "xunit.h"
#include "xunit/XUNIT.errtab.h"
#include "logger.h"

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

  int total_pass = 0;
  int total_fail = 0;
  //
  // allocated with the same size as g_args.objects
  void ** objects = 0;
  int x = 0;
  int token = 0;

  // load libraries
  fatal(logger_load);
  fatal(xlinux_load);
  fatal(xunit_load);

  // initialize logger
  fatal(logger_category_register, categories);
  fatal(logger_stream_register, streams);
  fatal(logger_initialize, envp);

  // parse cmdline arguments
  fatal(args_parse);

  // summarize
  fatal(args_summarize);

  // allocation for dloaded objects
  fatal(xmalloc, &objects, sizeof(*objects) * g_args.objectsl);

  for(;x < g_args.objectsl; x++)
  {
    // open the object
    fatal(xdlopen, g_args.objects[x], RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND, &objects[x]);

    // load the test manifest
    xunit_unit * xunit;
    fatal(uxdlsym, objects[x], "xunit", (void*)&xunit);

    if(xunit)
    {
      xunit_test ** test = xunit->tests;
      while(*test)
        test++;

      logf(L_DLOAD, "loaded %s -> xunit : { setup : %p, teardown : %p, release : %p, tests : %zu }"
        , g_args.objects[x]
        , xunit->setup 
        , xunit->teardown
        , xunit->release
        , test - xunit->tests
      );
    }
    else
    {
      logf(L_DLOAD, "loaded %s", g_args.objects[x]);    // not an object containing tests
    }

    if(xunit)
    {
      // unit setup
      if(xunit->setup)
        fatal(xunit->setup, xunit);

      xunit_test ** test = xunit->tests;

      int unit_pass = 0;
      int unit_fail = 0;
      int unit_tests = sentinel(test);

      // execute all tests
      while(*test)
      {
        // convenience
        (*test)->unit = xunit;

        int test_pass = 0;
        int test_fail = 0;

        if(invoke((*test)->entry, (*test)))
        {
          // add identifying info
          if((*test)->name)
            xapi_info_adds("test", (*test)->name);
          else
            xapi_info_addf("test", "%d", (test - xunit->tests) + 1);
          xapi_info_adds("object", g_args.objects[x]);

          // propagate non-unit-testing errors
          if(XAPI_ERRTAB != perrtab_XUNIT || XAPI_ERRCODE != XUNIT_FAIL)
            fail(0);

          // save the trace
          size_t z = xapi_trace_pithy(space, sizeof(space));

          // discard the error frames
          xapi_calltree_unwind();

          // for unit-testing errors, log the failure and continue
          logf(L_FAIL, "%*s %.*s", 3, "", (int)z, space);
          
          test_fail++;
        }
        else
        {
          test_pass++;
        }

        // report for this test
        fatal(log_xstart, L_TEST, test_pass ? L_GREEN : L_RED, &token);
        xlogf(0, test_pass ? L_GREEN : L_RED
          , "%7s %*s %s %*s (%3d / %-3d)"
          , "test"
          , 7, ""
          , test_pass ? "pass" : "fail"
          , 4, ""
          , (test - xunit->tests) + 1
          , unit_tests
        );

        if((*test)->name)
          logf(0, " %s", (*test)->name);
        fatal(log_finish, &token);

        unit_pass += test_pass;
        unit_fail += test_fail;

        test++;
      }

      // report for this module
      xlogf(L_UNIT, unit_fail == 0 ? L_GREEN : L_RED
        , "%6s %%%6.2f pass rate (%3d / %-3d) for %s"
        , "unit"
        , 100 * ((double)unit_pass / (double)(unit_pass + unit_fail))
        , unit_pass
        , unit_tests
        , g_args.objects[x]
      );

      total_pass += unit_pass;
      total_fail += unit_fail;

      // unit cleanup
      if(xunit->teardown)
        xunit->teardown(xunit);

      if(xunit->release)
        fatal(xunit->release, xunit);
    }
  }

  // summary report
  xlogf(L_SUMMARY, total_fail ? L_RED : L_GREEN
    , "%-5s %%%6.2f pass rate (%3d / %-3d)"
    , "suite"
    , 100 * ((double)total_pass / (double)(total_pass + total_fail))
    , total_pass
    , (total_pass + total_fail)
  );

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

  return R | total_fail;
}
