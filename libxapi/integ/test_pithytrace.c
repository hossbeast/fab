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
  char space[4096];
  size_t z;
#endif

  fatal(beta);

finally:
#if XAPI_STACKTRACE
  z = xapi_trace_pithy(space, sizeof(space));
  assertf(strstr(space, "foo=42"), "expected foo=42, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(strstr(space, "bar=27"), "expected bar=27, actual trace\n**\n%.*s\n**\n", (int)z, space);
#endif
coda;
}

static xapi epsilon()
{
  enter;

  xapi_fail_intent();
  xapi_info_adds("foo", "42");
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
  size_t z;
#endif

  fatal(epsilon);

finally:
#if XAPI_STACKTRACE
  xapi_infos("foo", "87");

  z = xapi_trace_pithy(space, sizeof(space));
  assertf(strstr(space, "foo=42"), "expected foo=42, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(!strstr(space, "foo=87"), "expected !foo=87, actual trace\n**\n%.*s\n**\n", (int)z, space);
#endif
coda;
}

static xapi lambda()
{
  enter;

  fail(TEST_ERROR_TWO);

finally:
  xapi_info_adds("bar", "38");
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
  size_t z;
#endif

  fatal(zeta);

finally:
#if XAPI_STACKTRACE
  z = xapi_trace_pithy(space, sizeof(space));
  assertf(strstr(space, "foo=42"), "expected foo=42, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(!strstr(space, "bar"), "expected !bar, actual trace\n**\n%.*s\n**\n", (int)z, space);
#endif
coda;
}

static xapi bar()
{
  enter;

  xapi_fail_intent();
  xapi_info_adds("foo", "42");
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
  char space[4096];
  size_t z;
#endif

  fatal(theta);

finally:
#if XAPI_STACKTRACE
  z = xapi_trace_pithy(space, sizeof(space));
  assertf(strstr(space, "foo=42"), "expected foo=42, actual trace\n**\n%.*s\n**\n", (int)z, space);
  assertf(!strstr(space, "bar"), "expected !bar, actual trace\n**\n%.*s\n**\n", (int)z, space);
#endif
coda;
}

int main()
{
  xapi exit = test_basic();
  assert_exit(TEST_ERROR_ONE, exit);

  exit = test_masking();
  assert_exit(TEST_ERROR_TWO, exit);

  exit = test_substack_0_skip();
  assert_exit(TEST_ERROR_ONE, exit);

  exit = test_substack_1_skip();
  assert_exit(TEST_ERROR_ONE, exit);

  succeed;
}
