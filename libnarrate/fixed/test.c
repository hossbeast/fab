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

#include "pstring.h"

#include "internal.h"
#include "dynamic.h"

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

static int call_vsayf(struct pstring ** restrict ps, const char * const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	fatal(dynamic_vsayf, ps, fmt, va);

finally:
	va_end(va);
coda;
}

//
// tests
//

// simple test designed to exercise dynamic writer functionality
static int test_small_entry(const narrate_test * test)
{
	pstring * ps = 0;

	// test input 
	char * input = "small";

	// narrate
	fatal(dynamic_sayw, &ps, MMS("sma"));
	fatal(call_vsayf, &ps, "%s", "ll");

	// verify
	if(ps == 0)
		failf(XUNIT_FAIL, "expected allocated, actual not allocated");

	if(ps->l != strlen(input))
		failf(XUNIT_FAIL, "expected size : %d, actual size : %d", strlen(input), ps->l);

	if(memcmp(ps->s, input, strlen(input)))
		failf(XUNIT_FAIL, "expected value : %s, actual value : %s", input, ps->s);

finally:
	psfree(ps);
coda;
}

// exercise dynamic writer functionality across reallocations
static int test_large_entry(const narrate_test * test)
{
	pstring * ps = 0;

	// test input part
#define VECTOR "abcdefghijklmnopqrstuvwxyz012345"	// 32 bytes

	// test input
	char * input = VECTOR VECTOR VECTOR VECTOR;

	// narrate
	fatal(dynamic_sayw, &ps, MMS(VECTOR));
	fatal(call_vsayf, &ps, "%s", VECTOR);
	fatal(dynamic_sayw, &ps, MMS(VECTOR));
	fatal(call_vsayf, &ps, "%s", VECTOR);

	// verify
	if(ps == 0)
		failf(XUNIT_FAIL, "expected allocated, actual not allocated");

	if(ps->l != strlen(input))
		failf(XUNIT_FAIL, "expected size : %d, actual size : %d", strlen(input), ps->l);

	if(memcmp(ps->s, input, strlen(input)))
		failf(XUNIT_FAIL, "expected value : %s, actual value : %s", input, ps->s);

finally:
	psfree(ps);
coda;
}

/* tests */

xunit_unit xunit = { tests : (xunit_test*[]) {
		  (narrate_test[]) {{ .entry = test_small_entry }}
		, (narrate_test[]) {{ .entry = test_large_entry }}
	}
};
