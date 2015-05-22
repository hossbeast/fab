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

static int test_entry(const narrate_test * test)
{
	pstring * ps = 0;

	// narrate
	fatal(dynamic_sayw, &ps, "a", 1);

	// verify
	if(ps == 0)
		failf(XUNIT_FAIL, "expected rows : %d, actual : %d", 1, 2);

	if(ps->l != 1)
		failf(XUNIT_FAIL, "expected rows : %d, actual : %d", 1, 2);

	if(memcmp(ps->s, "a", 1))
		failf(XUNIT_FAIL, "expected rows : %d, actual : %d", 1, 2);

finally:
	psfree(ps);
coda;
}

/* tests */

xunit_unit xunit = { tests : (xunit_test*[]) { (xunit_test[]) {{ .entry = test_entry }} }};
