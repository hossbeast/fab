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
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "narrator/growing.h"

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

  finally : coda;
}

static xapi test_basic()
{
  enter;

  narrator * N = 0;
  fatal(narrator_growing_create, &N);
  fatal(say, N);

  char * expected = "40 41 42 43 44";
  size_t expectedl = strlen(expected);

  assert_eq_s(expected, N->growing.s);
  assert_eq_zu(expectedl, N->growing.l);
  assert_eq_p(narrator_growing_buffer(N), N->growing.s);

finally:
  fatal(narrator_xfree, N);
coda;
}

static xapi test_seek()
{
  enter;

  narrator * N = 0;
  fatal(narrator_growing_create, &N);

  xsays("H");

  fatal(narrator_xseek, N, 2, NARRATOR_SEEK_SET, 0);
  xsays("L");

  fatal(narrator_xseek, N, 1, NARRATOR_SEEK_SET, 0);
  xsays("E");

  fatal(narrator_xseek, N, 4, NARRATOR_SEEK_SET, 0);
  xsays("O");

  fatal(narrator_xseek, N, 3, NARRATOR_SEEK_SET, 0);
  xsays("L");

  assert_eq_s("HELLO", N->growing.s);

finally:
  fatal(narrator_xfree, N);
coda;
}

static xapi test_init()
{
  enter;

  narrator * N = 0;
  char Nstor[NARRATOR_STATIC_SIZE];
  char * buf = 0;

  N = narrator_growing_init(Nstor);

  xsays("hello");
  xsays(" ");
  xsays("world");

  narrator_growing_claim_buffer(N, &buf, 0);

  assert_eq_s("hello world", buf);

finally:
  fatal(narrator_xdestroy, N);
  wfree(buf);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_basic);
  fatal(test_seek);
  fatal(test_init);

finally:
  summarize;
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
conclude(&R);

  xapi_teardown();
  return !!R;
}
