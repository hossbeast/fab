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
#include "logger.h"
#include "load.internal.h"

#include "internal.h"
#include "listwise_test.h"
#include "macros.h"

xapi listwise_test_setup(xunit_unit * unit)
{
  enter;

  // load libraries
  fatal(listwise_load);

  // re-initialize logging
  fatal(logger_finalize);

  finally : coda;
}

xapi listwise_test_cleanup(xunit_unit * unit)
{
  enter;

  fatal(listwise_unload);

  finally : coda;
}

API xapi listwise_test_entry(listwise_test * test)
{
  enter;

  int token = 0;
  lwx * lx = 0;
  int i = 0;
  int x = 0;

#if 0
  // log in the inset
  fatal(log_start, L_INSET, &token);
  if(token)
  {
    logs(0, "[ ");
    for(x = 0; x < sentinel(test->init); x++)
    {
      if(x)
        logs(0, ", ");
      logs(0, test->init[x]);
    }
    logf(0, " ~ %s ]", test->xsfm);
  }
  fatal(log_finish, &token);
#endif

  fatal(listwise_exec, test->xsfm, test->init, 0, sentinel(test->init), &lx);

#if 0
  // log the outset
  fatal(log_start, L_OUTSET, &token);
  if(token)
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
  }
  fatal(log_finish, &token);
#endif

  // compare
  i = 0;
  x = 0;
  LSTACK_ITERATE(lx, x, go)
  if(go)
  {
    char * s;
    fatal(lstack_string, lx, 0, x, &s);

    if(i < sentinel(test->final))
      assert_eq_s(test->final[i], s);

    i++;
  }
  LSTACK_ITEREND

  assert_eq_d(sentinel(test->final), i);

finally:
  fatal(log_finish, &token);
  lwx_free(lx);
coda;
}
