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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "xapi/SYS.errtab.h"

#include "xapi.h"
#include "types.h"

#include "xlinux/load.h"
#include "logger/load.h"
#include "xunit/load.h"
#include "narrator/load.h"

#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xtime.h"
#include "xlinux/xdlfcn.h"
#include "xlinux/xwait.h"
#include "xlinux/xresource.h"
#include "xlinux/xunistd.h"
#include "xunit/assert.h"
#include "xunit/XUNIT.errtab.h"
#include "logger.h"
#include "narrator.h"
#include "narrator/units.h"

#include "MAIN.errtab.h"
#include "args.h"
#include "cores.h"
#include "common/assure.h"
#include "logging.h"
#include "macros.h"

static inline double pct(double a, double b)
{
  if((a + b) == 0)
    return 0;

  return 100 * (a / (a + b));
}

static xapi main_exit;
static uint32_t suite_assertions_passed;
static uint32_t suite_assertions_failed;
static uint32_t suite_tests_passed;
static uint32_t suite_tests_failed;
static uint32_t suite_units;

typedef struct test_results
{
  xapi status;
  uint32_t assertions_passed;
  uint32_t assertions_failed;
} test_results;

static xapi xmain()
{
  enter;

  char trace[4096];
  Dl_info nfo;
  narrator * N;
  uint64_t * test_vector = 0;
  size_t test_vector_l = 0;
  size_t test_vector_a = 0;
  test_results * results = 0;
  pid_t pid;
  int wstatus;

  // allocated with the same size as g_args.objects
  void ** objects = 0;
  int x = 0;

  // parse cmdline arguments
  fatal(args_parse);
  fatal(args_summarize);

  // enable coredumps
  setrlimit(RLIMIT_CORE, (struct rlimit[]) {{ .rlim_cur = RLIM_INFINITY, .rlim_max = RLIM_INFINITY }});

  // allocation for dloaded objects
  fatal(xmalloc, &objects, sizeof(*objects) * g_args.objectsl);

  if((results = mmap(0, sizeof(test_results), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == 0)
    fail(KERNEL_ENOMEM);

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
/* asan does not work with DEEPBIND */
//      | RTLD_DEEPBIND     // cause this object to prefer symbols which it contains
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

      test_vector_l = 0;

      xunit_test ** test = xunit->xu_tests;

      uint32_t unit_tests_passed = 0;
      uint32_t unit_tests_failed = 0;
      uint32_t unit_assertions_passed = 0;
      uint32_t unit_assertions_failed = 0;

      int maxweight = ~0;
      while(*test)
      {
        // convenience
        (*test)->xu_unit = xunit;

        int weight = (*test)->xu_weight;
        if(weight > maxweight)
          maxweight = weight;

        test++;
      }
      size_t tests_len = (test - (xunit_test**)xunit->xu_tests) - 1;

      // execute all tests
      test = xunit->xu_tests;
      for(test = xunit->xu_tests; *test; test++)
      {
        (*test)->xu_index = (int)(test - (xunit_test**)xunit->xu_tests);

        int weight = (*test)->xu_weight;
        if(weight != maxweight)
          continue;

        // convenience
        (*test)->xu_unit = xunit;

        xunit_test_entry entry = (*test)->xu_entry ?: xunit->xu_entry;

        const char * name = 0;
        if(!(name = (*test)->xu_name))
        {
          int r = dladdr(entry, &nfo);
          if(r && nfo.dli_sname)
            name = nfo.dli_sname;
        }

        struct timespec test_start;
        struct timespec test_end;
        fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &test_start);

        xunit_assertions_passed = 0;
        xunit_assertions_failed = 0;

        memset(results, 0, sizeof(test_results));

        if(g_args.fork)
        {
          fatal(xfork, &pid);
        }

        if(!g_args.fork || pid == 0)
        {
          // unit setup
          if(xunit->xu_setup)
            fatal(xunit->xu_setup, xunit);

          if((results->status = invoke(entry, *test)))
          {
            // add identifying info
            if(name)
              xapi_frame_info_pushs("name", name);
            xapi_frame_info_pushf("test", "%d in [%d,%zu]", (*test)->xu_index, 0, tests_len);

            // propagate non-unit-testing errors
            if(XAPI_ERRVAL != XUNIT_FAIL)
              fail(0);

            // for unit-testing errors, log the failure and continue
            xapi_trace_full(trace, sizeof(trace), XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
            xapi_calltree_unwind();
            logs(L_FAIL, trace);
          }

          results->assertions_passed = xunit_assertions_passed;
          results->assertions_failed = xunit_assertions_failed;

          // unit cleanup
          if(xunit->xu_teardown)
            xunit->xu_teardown(xunit);

          if(xunit->xu_cleanup)
            fatal(xunit->xu_cleanup, xunit);

          if(g_args.fork)
          {
            // in the child, ignore suite status
            suite_assertions_failed = 0;
            suite_tests_failed = 0;

            goto XAPI_FINALIZE;
          }
        }

        wstatus = 0;
        if(g_args.fork)
          fatal(xwaitpid, pid, &wstatus, 0);

        fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &test_end);

        xunit_assertions_passed = results->assertions_passed;
        xunit_assertions_failed = results->assertions_failed;
        bool test_failed = xunit_assertions_failed || wstatus || results->status;

        if(WIFEXITED(wstatus))
        {
          int exit = WEXITSTATUS(wstatus);
          if(exit != 0)
            xlogf(L_FAIL, L_RED, "exited with status %d", exit);
        }
        else if(WIFSIGNALED(wstatus))
        {
          int sig = WTERMSIG(wstatus);
          bool core = WCOREDUMP(wstatus);
          if(core)
            fatal(print_core_backtrace, pid);
          xlogf(L_FAIL, L_RED, "exited with signal %d, core %s", sig, core ? "yes" : "no");
        }
        else
        {
          xlogs(L_FAIL, L_RED, "exited abnormally");
        }

        // report for this test
        fatal(log_xstart, L_TEST, test_failed ? L_RED : L_GREEN, &N);
        xsayf("  %%%6.2f pass rate on %6"PRIu32" assertions by test %d in [%d,%zu] over "
          , pct(xunit_assertions_passed, xunit_assertions_failed)
          , xunit_assertions_passed + xunit_assertions_failed
          , (*test)->xu_index
          , 0
          , tests_len
        );
        fatal(elapsed_say, &test_start, &test_end, N);
        if(name)
          xsayf(" for %s", name);

        fatal(log_finish);

        if(!test_failed)
        {
          unit_tests_passed++;
        }
        else
        {
          unit_tests_failed++;

          div_t r = div((*test)->xu_index, (sizeof(*test_vector) * 8));
          int i = r.quot;
          int o = r.rem;
          fatal(assure, &test_vector, sizeof(*test_vector), i, &test_vector_a);
          if(test_vector_l <= i)
          {
            memset(
                test_vector + test_vector_l
              , 0
              , (i - test_vector_l + 1) * sizeof(*test_vector)
            );
            test_vector_l = i + 1;
          }
          test_vector[i] |= 1ULL << o;
        }

        unit_assertions_passed += xunit_assertions_passed;
        unit_assertions_failed += xunit_assertions_failed;
      }

      fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &unit_end);

      // report for this module
      if(unit_tests_failed)
      {
        fatal(log_xstart, L_UNIT, L_RED, &N);
        xsayf("   %6d failed test(s) :", unit_tests_failed);
        int x;
        for(x = 0; x < test_vector_l; x++)
        {
          int y;
          for(y = 0; y < (sizeof(*test_vector) * 8); y++)
          {
            if(test_vector[x] & (1ULL << y))
            {
              xsayf(" %zu", (x * (sizeof(*test_vector) * 8)) + y);
            }
          }
        }
        xsayf(" in [%d,%zu]", 0, tests_len);
        fatal(log_finish);
      }

      fatal(log_xstart, L_UNIT, unit_tests_failed ? L_RED : L_GREEN, &N);
      xsayf(" %%%6.2f pass rate on %6"PRIu32" assertions by %"PRIu32" tests in [%d,%zu] over "
        , pct(unit_assertions_passed, unit_assertions_failed)
        , unit_assertions_passed + unit_assertions_failed
        , unit_tests_passed + unit_tests_failed
        , 0
        , tests_len
      );

      fatal(elapsed_say, &unit_start, &unit_end, N);
      xsayf(" in %s", g_args.objects[x]);
      fatal(log_finish);

      suite_assertions_passed += unit_assertions_passed;
      suite_assertions_failed += unit_assertions_failed;
      suite_tests_passed += unit_tests_passed;
      suite_tests_failed += unit_tests_failed;
      suite_units++;
    }
  }

  fatal(xclock_gettime, CLOCK_MONOTONIC_RAW, &suite_end);

  // summary report
  fatal(log_xstart, L_SUITE, suite_tests_failed ? L_RED : L_GREEN, &N);
  xsayf("%%%6.2f pass rate on %6d assertions by %"PRIu32" tests in [%d,%d] from %4d units over "
    , pct(suite_assertions_passed, suite_assertions_failed)
    , (suite_assertions_passed + suite_assertions_failed)
    , suite_tests_passed + suite_tests_failed
    , 0
    , suite_tests_passed + suite_tests_failed
    , suite_units
  );

  fatal(elapsed_say, &suite_start, &suite_end, N);
  fatal(log_finish);

finally:
  // locals
  if(objects)
  {
    for(x = 0; x < g_args.objectsl; x++)
      fatal(ixdlclose, &objects[x]);
  }
  wfree(objects);
  wfree(test_vector);

  // modules
  args_teardown();
coda;
}

static xapi main_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
    main_exit = XAPI_ERRVAL;
    fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE);
    xapi_calltree_unwind();
  }
coda;
}

static xapi main_load(char ** envp)
{
  enter;

  // load libraries
  fatal(logger_load);
  fatal(xunit_load);

  // initialize logger
  fatal(logging_setup, envp);

  // main program
  fatal(main_jump);

finally:
  // libraries
  fatal(logger_unload);
  fatal(xunit_unload);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  xapi R = 0;
  fatal(main_load, envp);

finally:
  if(XAPI_UNWINDING)
  {
    // failures which cannot be logged with liblogger to stderr
    xapi_backtrace();
  }

conclude(&R);
  xapi_teardown();

  R |= main_exit;
  R |= suite_assertions_failed;
  R |= suite_tests_failed;

  return !!R;
}

