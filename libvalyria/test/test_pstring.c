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

#include "pstring.h"

#include "test_util.h"

xapi validate(pstring * ps)
{
  enter;

  assertf(ps->l == 10, "length %zu", "length %zu", 10, ps->l);
  assertf(strcmp(ps->s, "1234567890") == 0, "content %s", "content %s", "1234567890", ps->s);

  finally : coda;
}

int main()
{
  enter;

  xapi r;
  pstring * ps = 0;
  fatal(pscreate, &ps);
  fatal(psloads, ps, "1234567890");
  fatal(validate, ps);

  fatal(psclear, ps);
  fatal(psloadc, ps, '1');
  fatal(pscatf, ps, "%s", "234567890");
  fatal(validate, ps);

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }

  psfree(ps);
conclude(&r);

  xapi_teardown();
  return !!r;
}
