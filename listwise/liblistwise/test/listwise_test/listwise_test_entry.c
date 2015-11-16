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

#include "xapi.h"

#include "internal.h"
#include "listwise_test.h"
#include "macros.h"

API xapi listwise_test_entry(const listwise_test * test)
{
	enter;

	lwx * lx = 0;
	int i = 0;
	int x = 0;

  // liblistwise initialization (idempotent)
  fatal(listwise_setup);

	// log in the inset
	if(log_start(L_XUNIT | L_INSET))
	{
		logs(0, "[ ");
		for(x = 0; x < sentinel(test->init); x++)
		{
			if(x)
				logs(0, ", ");
			logs(0, test->init[x]);
		}
		logf(0, " ~ %s ]", test->xsfm);
		log_finish();
	}

	fatal(listwise_exec, test->xsfm, 0, test->init, 0, sentinel(test->init), &lx);

	// log the outset
	if(log_start(L_XUNIT | L_OUTSET))
	{
		logs(0, "[ ");

		i = 0;
		x = 0;
		LSTACK_ITERATE(lx, x, go)
		if(go)
		{
			char * s;
			fatal(lstack_string, lx, 0, x, &s);

			if(i)
				logs(0, ", ");
			logs(0, s);
			i++;
		}
		LSTACK_ITEREND

		logs(0, " ]");
		log_finish();
	}

	// compare
	i = 0;
	x = 0;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * s;
		fatal(lstack_string, lx, 0, x, &s);

		if(i < sentinel(test->final))
		{
			if(strcmp(test->final[i], s))
			{
				failf(XUNIT_FAIL, "expected row : %s, actual : %s", test->final[i], s);
			}
		}

		i++;
	}
	LSTACK_ITEREND

	if(i != sentinel(test->final))
	{
		failf(XUNIT_FAIL, "expected rows : %d, actual : %d", sentinel(test->final), i);
	}

finally:
	lwx_free(lx);
coda;
}
