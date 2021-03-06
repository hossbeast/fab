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

#include "narrator.h"
#include "narrator/fixed.h"

#include "test_util.h"

static xapi say(narrator * const N)
{
  enter;

  // says the string : 40 41 42 43 44
  xsayf("%d", 40);
  xsays(" 41");
  xsayw(" 42", 3);
  xsayf(" %d", 43);
  xsayf(" %d", 4);
  xsayc('4');
  fatal(narrator_flush, N);

  finally : coda;
}

static xapi test_basic()
{
  enter;

  char s[64];
  narrator_fixed nf;
  narrator *N;

  narrator_fixed_init(&nf, s, sizeof(s));
  N = &nf.base;

  fatal(say, N);

  char * expected = "40 41 42 43 44";
  size_t expectedl = strlen(expected);

  assert_eq_s(expected, nf.s);
  assert_eq_zu(expectedl, nf.l);

  finally : coda;
}

static xapi test_constrained()
{
  enter;

  char s[7];
  narrator_fixed nf;

  narrator * N = narrator_fixed_init(&nf, s, sizeof(s));
  fatal(say, N);

  char * expected = "40 41 ";
  size_t expectedl = 6;

  assert_eq_zu(expectedl, nf.l);
  assert_eq_s(expected, nf.s);

  finally : coda;
}

int main()
{
  enter;

  xapi R = 0;

  fatal(test_basic);
  fatal(test_constrained);

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
