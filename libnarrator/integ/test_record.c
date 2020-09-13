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
#include <stdarg.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "narrator.h"
#include "narrator/record.h"
#include "narrator/growing.h"

#include "test_util.h"

static xapi test_fatal()
{
  enter;

  narrator * N;
  narrator_growing * ng = 0;
  narrator_record *nr = 0;
  fatal(narrator_growing_create, &ng);
  fatal(narrator_record_create, &nr, &ng->base);
  N = &nr->base;

  xsays("hello");
  xsayc(' ');
  xsays("world");
  fatal(narrator_record_flush, nr);

  assert_eq_s("hello world", ng->s);

finally:
  fatal(narrator_record_free, nr);
  fatal(narrator_growing_free, ng);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_fatal);

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
