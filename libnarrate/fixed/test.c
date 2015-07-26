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

#include "internal.h"
#include "fixed/fixed.internal.h"

#include "macros.h"

struct narrate_test;
#define TEST_TYPE struct narrate_test
#include "xunit.h"

typedef struct narrate_test
{
  xunit_test;
} narrate_test;

#define restrict __restrict

//
// static
//

static int call_vsayf(char * const restrict dst, size_t sz, size_t * const restrict szo, const char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  fatal(fixed_vsayf, dst, sz, szo, fmt, va);

finally:
  va_end(va);
coda;
}

//
// tests
//

// does not fill the entire buffer
static int entry_small(const narrate_test * test)
{
  char buf[16];
  size_t sz = sizeof(buf);
  size_t szo = 0;

  // test input 
  char * input = "small";

  // narrate
  fatal(fixed_sayw, buf, sz, &szo, MMS("sma"));
  fatal(call_vsayf, buf, sz, &szo, "%s", "ll");

  // verify
  if(szo >= sz)
    failf(XUNIT_FAIL, "expected in-range (0, %zu], actual : %zu", sz, szo);

  if(szo != strlen(input))
    failf(XUNIT_FAIL, "expected written : %d, actual written : %d", strlen(input), szo);

  if(memcmp(buf, input, strlen(input)))
    failf(XUNIT_FAIL, "expected value : %s, actual value : %s", input, buf);

  finally : coda;
}

// fills the buffer exactly
static int entry_boundary(const narrate_test * test)
{
  char buf[6];
  size_t sz = sizeof(buf);
  size_t szo = 0;

  // test input 
  char * input = "small";

  // narrate
  fatal(fixed_sayw, buf, sz, &szo, MMS("sma"));
  fatal(call_vsayf, buf, sz, &szo, "%s", "ll");

  // verify
  if(szo >= sz)
    failf(XUNIT_FAIL, "expected in-range (0, %zu], actual : %zu", sz, szo);

  if(szo != strlen(input))
    failf(XUNIT_FAIL, "expected value : %s, actual value : %.*s", input, (int)szo, buf);

  if(memcmp(buf, input, strlen(input)))
    failf(XUNIT_FAIL, "expected value : %s, actual value : %.*s", input, (int)szo, buf);

  finally : coda;
}

// overflows the specified buffer
static int entry_overflow(const narrate_test * test)
{
  char buf[128];
  size_t sz = sizeof(buf);
  size_t szo = 0;

  // narrate
  int x;
  for(x = 0; x < 10; x++)
  {
    fatal(fixed_sayw, buf, sz, &szo, MMS("abcdefghijklmnopqrstuvwxyz0123456789"));
    fatal(call_vsayf, buf, sz, &szo, "%s", "ll");
  }

  // verify
  if(szo != (sz - 1))
    failf(XUNIT_FAIL, "expected written %zu, actual : %zu", sz, szo);

  finally : coda;
}

/* tests */
xunit_unit xunit = { tests : (xunit_test*[]) {
      (narrate_test[]) {{ .entry = entry_small }}
    , (narrate_test[]) {{ .entry = entry_boundary }}
    , (narrate_test[]) {{ .entry = entry_overflow }}
  }
};
