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

#include <unistd.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "test.h"

/*

SUMMARY
 contents of the pithytrace

*/

static xapi fi()
{
  enter;

  fail(TEST_ERROR_ONE);

finally:
  xapi_infof("foo", "%d", 42);
  xapi_infof("bar", "%d", 27);
coda;
}

static xapi beta()
{
  enter;

  fatal(fi);

  finally : coda;
}

/// test_basic
//
// SUMMARY
//  verify infos from other frames are present
//
static xapi test_basic()
{
  enter;

#if XAPI_STACKTRACE
  char value[4096];
#endif

  fatal(beta);

finally:
#if XAPI_STACKTRACE
  xapi_trace_info("foo", value, sizeof(value));
  assert_eq_s("42", value);

  xapi_trace_info("bar", value, sizeof(value));
  assert_eq_s(value, "27");
#endif
coda;
}

static xapi epsilon()
{
  enter;

  xapi_info_pushs("foo", "42");
  fail(TEST_ERROR_TWO);

  finally : coda;
}

/// test_masking
//
// SUMMARY
//  verify that infos from lower frames mask infos in higher frames with the same key
//
static xapi test_masking()
{
  enter;

#if XAPI_STACKTRACE
  char space[4096];
#endif

  fatal(epsilon);

finally:
#if XAPI_STACKTRACE
  xapi_infos("foo", "87");

  xapi_trace_pithy(space, sizeof(space), 0);
  assertf(strstr(space, "foo 42"), "expected foo 42, actual trace\n**\n%s\n**\n", space);
  assertf(!strstr(space, "foo 87"), "expected !foo 87, actual trace\n**\n%s\n**\n", space);
#endif
coda;
}

static xapi lambda()
{
  enter;

  fail(TEST_ERROR_TWO);

finally:
  xapi_info_pushs("bar", "38");
coda;
}

static xapi zeta()
{
  enter;

  fail(TEST_ERROR_ONE);

finally:
  xapi_infos("foo", "42");
  fatal(lambda);
coda;
}

/// test_substack_0_skip
//
// SUMMARY
//  verify infos are skipped in a substack rooted at frame 0
//
static xapi test_substack_0_skip()
{
  enter;

#if XAPI_STACKTRACE
  char space[4096];
#endif

  fatal(zeta);

finally:
#if XAPI_STACKTRACE
  xapi_trace_pithy(space, sizeof(space), 0);
  assertf(strstr(space, "foo 42"), "expected foo 42, actual trace\n**\n%s\n**\n", space);
  assertf(!strstr(space, "bar"), "expected !bar, actual trace\n**\n%s\n**\n", space);
#endif
coda;
}

static xapi bar()
{
  enter;

  xapi_info_pushs("foo", "42");
  fail(TEST_ERROR_ONE);

  finally : coda;
}

static xapi theta()
{
  enter;

  fatal(bar);

finally:
  fatal(lambda);
coda;
}

/// test_substack_1_skip
//
// SUMMARY
//  verify infos are skipped in a substack rooted at frame 1
//
static xapi test_substack_1_skip()
{
  enter;

#if XAPI_STACKTRACE
  char value[4096];
#endif

  fatal(theta);

finally:
#if XAPI_STACKTRACE
  xapi_trace_info("foo", value, sizeof(value));
  assert_eq_s("42", value);

  size_t info_len = xapi_trace_info("bar", 0, 0);
  assert_eq_zu(0, info_len);
#endif
coda;
}

int main()
{
  xapi exit = test_basic();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  exit = test_masking();
  assert_eq_exit(TEST_ERROR_TWO, exit);

  exit = test_substack_0_skip();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  exit = test_substack_1_skip();
  assert_eq_exit(TEST_ERROR_ONE, exit);

  succeed;
}
