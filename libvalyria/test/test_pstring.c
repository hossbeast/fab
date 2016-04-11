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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

#include "pstring.internal.h"

#include "test_util.h"

xapi validate(pstring * ps)
{
  enter;

  assertf(ps->a > ps->l, "allocated %zu", "length %zu", ps->a, ps->l);

  finally : coda;
}

xapi assert_contents(pstring * ps)
{
  enter;

  fatal(validate, ps);
  assertf(ps->l == 10, "length %zu", "length %zu", 10, ps->l);
  assertf(strcmp(ps->s, "1234567890") == 0, "content %s", "content %s", "1234567890", ps->s);

  finally : coda;
}

xapi test_basic()
{
  enter;

  pstring * ps = 0;

  fatal(pscreate, &ps);
  fatal(psloads, ps, "1234567890");
  fatal(assert_contents, ps);

  fatal(psclear, ps);
  fatal(psloadc, ps, '1');
  fatal(pscatf, ps, "%s", "234567890");
  fatal(assert_contents, ps);

finally:
  psfree(ps);
coda;
}

xapi test_load()
{
  enter;

  pstring * ps = 0;

  fatal(pscreate, &ps);

  int x;
  for(x = 0; x < 25; x++)
  {
    fatal(pscats, ps, " "); fatal(validate, ps);
    fatal(pscatc, ps, ' '); fatal(validate, ps);
    fatal(pscatf, ps, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); fatal(validate, ps);
    fatal(pscatf, ps, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); fatal(validate, ps);
    fatal(pscatf, ps, "%s %s %s %s", "alpha", "beta", "delta", "gamma"); fatal(validate, ps);
  }

finally:
  psfree(ps);
coda;
}

int main()
{
  enter;
  xapi R;

  fatal(test_basic);
  fatal(test_load);

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }

conclude(&R);

  xapi_teardown();
  return !!R;
}
