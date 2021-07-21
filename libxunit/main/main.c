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

#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "types.h"

#include "narrator/load.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xtime.h"
#include "xlinux/xwait.h"
#include "xlinux/xresource.h"
#include "xlinux/xunistd.h"
#include "xlinux/xmman.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/units.h"
#include "common/color.h"

#include "args.h"
#include "unit.h"
#include "cores.h"
#include "common/assure.h"
#include "macros.h"

__thread int32_t tid;

static inline double pct(double a, double b)
{
  if((a + b) == 0)
    return 0;

  return 100 * (a / (a + b));
}

static uint32_t suite_assertions_passed;
static uint32_t suite_assertions_failed;
static uint32_t suite_tests_passed;
static uint32_t suite_tests_failed;
static uint32_t suite_units;

typedef struct test_results
{
  int status;
  uint32_t assertions_passed;
  uint32_t assertions_failed;
} test_results;

static void xmain(int argc, char ** argv)
{
  uint64_t * test_vector = 0;
  size_t test_vector_l = 0;
  size_t test_vector_a = 0;
  test_results * results = 0;
  pid_t pid = 0;
  int wstatus;
  xunit_unit *xunit;
  size_t units;

  // parse cmdline arguments
  args_parse(argc, argv);

  // enable coredumps
  setrlimit(RLIMIT_CORE, (struct rlimit[]) {{ .rlim_cur = RLIM_INFINITY, .rlim_max = RLIM_INFINITY }});

  // allocation for dloaded objects
  results = xmmap(0, sizeof(test_results), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  struct timespec suite_start;
  struct timespec suite_end;
  xclock_gettime(CLOCK_MONOTONIC_RAW, &suite_start);

  units = 0;
  units_foreach(xunit) {
    units++;
    struct timespec unit_start;
    struct timespec unit_end;
    xclock_gettime(CLOCK_MONOTONIC_RAW, &unit_start);

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
      struct timespec test_start;
      struct timespec test_end;
      xclock_gettime(CLOCK_MONOTONIC_RAW, &test_start);

      xunit_assertions_passed = 0;
      xunit_assertions_failed = 0;

      memset(results, 0, sizeof(test_results));

      if(g_args.fork)
      {
        pid = xfork();
        if(pid == 0) {
          tid = syscall(SYS_gettid);
        }
      }

      if(!g_args.fork || pid == 0)
      {
        // unit setup
        if(xunit->xu_setup)
          xunit->xu_setup(xunit);

        entry(*test);
#if 0
        if((results->status = entry(*test)))
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
#endif

        results->assertions_passed = xunit_assertions_passed;
        results->assertions_failed = xunit_assertions_failed;

        // unit cleanup
        if(xunit->xu_teardown)
          xunit->xu_teardown(xunit);

        if(xunit->xu_cleanup)
          xunit->xu_cleanup(xunit);

        if(g_args.fork)
        {
          // in the child, ignore suite status
          suite_assertions_failed = 0;
          suite_tests_failed = 0;

          goto end;
        }
      }

      wstatus = 0;
      if(g_args.fork) {
        xwaitpid(pid, &wstatus, 0);
      }

      xclock_gettime(CLOCK_MONOTONIC_RAW, &test_end);

      xunit_assertions_passed = results->assertions_passed;
      xunit_assertions_failed = results->assertions_failed;
      bool test_failed = xunit_assertions_failed || wstatus || results->status;

      if(WIFEXITED(wstatus))
      {
        int exit = WEXITSTATUS(wstatus);
        if(exit != 0) {
          write(2, RED);
          dprintf(2, "exited with status%d\n", exit);
          write(2, NOCOLOR);
        }
      }
      else if(WIFSIGNALED(wstatus))
      {
        int sig = WTERMSIG(wstatus);
        bool core = WCOREDUMP(wstatus);
        if(core)
          print_core_backtrace(pid);
        write(2, RED);
        dprintf(2, "exited with signal %d, core %s\n", sig, core ? "yes" : "no");
        write(2, NOCOLOR);
      }
      else
      {
        write(2, RED);
        dprintf(2, "exited abnormally\n");
        write(2, NOCOLOR);
      }

      // report for this test
      if(test_failed) {
        write(2, RED);
      } else {
        write(2, GREEN);
      }
      dprintf(2, "  %%%6.2f pass rate on %6"PRIu32" assertions by test %d in [%d,%zu] over "
        , pct(xunit_assertions_passed, xunit_assertions_failed)
        , xunit_assertions_passed + xunit_assertions_failed
        , (*test)->xu_index
        , 0
        , tests_len
      );
      elapsed_say(&test_start, &test_end, g_narrator_stderr);
      if(name) {
        dprintf(2, " for %s", name);
      }
      write(2, NOCOLOR);
      dprintf(2, "\n");

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
        assure(&test_vector, sizeof(*test_vector), i, &test_vector_a);
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

    xclock_gettime(CLOCK_MONOTONIC_RAW, &unit_end);

    // report for this module
    if(unit_tests_failed)
    {
      write(2, RED);
      dprintf(2, "   %6d failed test(s) :", unit_tests_failed);
      int x;
      for(x = 0; x < test_vector_l; x++)
      {
        int y;
        for(y = 0; y < (sizeof(*test_vector) * 8); y++)
        {
          if(test_vector[x] & (1ULL << y))
          {
            dprintf(2, " %zu", (x * (sizeof(*test_vector) * 8)) + y);
          }
        }
      }
      dprintf(2, " in [%d,%zu]\n", 0, tests_len);
      write(2, NOCOLOR);
    }

    if(unit_tests_failed) {
      write(2, RED);
    } else {
      write(2, GREEN);
    }
    dprintf(2, " %%%6.2f pass rate on %6"PRIu32" assertions by %"PRIu32" tests in [%d,%zu] over "
      , pct(unit_assertions_passed, unit_assertions_failed)
      , unit_assertions_passed + unit_assertions_failed
      , unit_tests_passed + unit_tests_failed
      , 0
      , tests_len
    );

    elapsed_say(&unit_start, &unit_end, g_narrator_stderr);
    dprintf(2, "\n");
    write(2, NOCOLOR);

    suite_assertions_passed += unit_assertions_passed;
    suite_assertions_failed += unit_assertions_failed;
    suite_tests_passed += unit_tests_passed;
    suite_tests_failed += unit_tests_failed;
    suite_units++;
  }

  xclock_gettime(CLOCK_MONOTONIC_RAW, &suite_end);

  // summary report
  if(suite_tests_failed) {
    write(2, RED);
  } else {
    write(2, GREEN);
  }
  dprintf(2,"%%%6.2f pass rate on %6d assertions by %"PRIu32" tests in [%d,%d] from %4d units over "
    , pct(suite_assertions_passed, suite_assertions_failed)
    , (suite_assertions_passed + suite_assertions_failed)
    , suite_tests_passed + suite_tests_failed
    , 0
    , suite_tests_passed + suite_tests_failed
    , suite_units
  );

  elapsed_say(&suite_start, &suite_end, g_narrator_stderr);
  dprintf(2, "\n");

end:
  // locals
  wfree(test_vector);
}

int main(int argc, char ** argv)
{
  int R;

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  tid = syscall(SYS_gettid);

  narrator_load();

  xmain(argc, argv);

  R = 0;
  R |= suite_assertions_failed;
  R |= suite_tests_failed;

  return !!R;
}
