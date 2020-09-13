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

#include "internal.h"
#include "narrator/growing.h"
#include "narrator/units.h"

#include "test_util.h"

static xapi test_interval_say()
{
  enter;

  // arrange
  narrator * N = 0;
  fatal(narrator_growing_create, &N);

  struct {
    time_t seconds;
    char * expected;
  } tests[] = {
      { seconds : 75          , expected : "1 min(s) 15 sec(s)" }
    , { seconds : 1           , expected : "1 sec(s)" }
    , { seconds : 0           , expected : "0 sec(s)" }
    , { seconds : 1234        , expected : "20 min(s) 34 sec(s)" }
    , { seconds : 3600        , expected : "1 hr(s) 0 sec(s)" }
    , { seconds : 86400       , expected : "1 day(s) 0 sec(s)" }
    , { seconds : 0xbeef      , expected : "13 hr(s) 34 min(s) 39 sec(s)" }
    , { seconds : 0xdeadbeef  , expected : "118 yr(s) 169 day(s) 21 hr(s) 55 min(s) 59 sec(s)" }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(narrator_xreset, N);

    // act
    fatal(interval_say, tests[x].seconds, N);

    // assert
    assert_eq_s(tests[x].expected, N->growing.s);
  }

finally:
  fatal(narrator_xfree, N);
coda;
}

static xapi test_bytesize_say()
{
  enter;

  // arrange
  narrator * N = 0;
  fatal(narrator_growing_create, &N);

  struct {
    size_t bytes;
    char * expected;
  } tests[] = {
      { bytes : 75          , expected : "75 byte(s)" }
    , { bytes : 1           , expected : "1 byte(s)" }
    , { bytes : 0xdeadbeef  , expected : "3G 490M 879K 751 byte(s)" }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(narrator_xreset, N);

    // act
    fatal(bytesize_say, tests[x].bytes, N);

    // assert
    assert_eq_s(tests[x].expected, N->growing.s);
  }

finally:
  fatal(narrator_xfree, N);
coda;
}

static xapi test_elapsed_say()
{
  enter;

  // arrange
  narrator * N = 0;
  fatal(narrator_growing_create, &N);

  struct {
    struct timespec start;
    struct timespec end;
    char * expected;
  } tests[] = {
      { start : { .tv_sec = 0 }, end : { .tv_sec = 100 }, expected : "1 min(s) 40.00 sec(s)" }
    , { start : { .tv_sec = 50, .tv_nsec = 500 }, end : { .tv_sec = 50, .tv_nsec = 0xdeadbeef }, expected : "3.74 sec(s)" }
    , { start : { .tv_sec = 50, .tv_nsec = 500 }, end : { .tv_sec = 50, .tv_nsec = 75 * 10000000 }, expected : "0.75 sec(s)" }
    , { start : { .tv_sec = 500, .tv_nsec = 500 }, end : { .tv_sec = 0xdeadbeef, .tv_nsec = 75 * 10000000 }, expected : "118 yr(s) 169 day(s) 21 hr(s) 47 min(s) 39.75 sec(s)" }
  };

  int x;
  for(x = 0; x < sizeof(tests) / sizeof(tests[0]); x++)
  {
    fatal(narrator_xreset, N);

    // act
    fatal(elapsed_say, &tests[x].start, &tests[x].end, N);

    // assert
    assert_eq_s(tests[x].expected, N->growing.s);
  }

finally:
  fatal(narrator_xfree, N);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_interval_say);
  fatal(test_bytesize_say);
  fatal(test_elapsed_say);

finally:
  summarize;
  if(XAPI_UNWINDING)
  {
    xapi_backtrace(2, 0);
  }
conclude(&R);

  xapi_teardown();
  return !!R;
}
