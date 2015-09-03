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

#include "test.h"
#include "memblk.def.h"

#include "xapi/calltree.h"

/*

SUMMARY
 test the functionality of xapi_calltree_freeze

*/

int beta(int num)
{
  enter;

  fail(SYS_ERESTART);

  finally : coda;
}

int alpha(int num)
{
  enter;

  fatal(beta, num);

  finally : coda;
}

char space[4096 << 2];
char space2[4096];

int foo()
{
  enter;

#if XAPI_MODE_STACKTRACE
  memblk * mb;
#endif

  fatal(alpha, 125);

finally:
#if XAPI_MODE_STACKTRACE
  mb = xapi_calltree_freeze();
  memblk_copyto(mb, space, sizeof(space));
  xapi_calltree_unfreeze();
#endif
coda;
}

int main()
{
  int exit = foo();
  assert_etab(perrtab_SYS);
  assert_code(SYS_ERESTART);

#if XAPI_MODE_STACKTRACE
  struct stack * cs = xapi_calltree_thaw(space);
  size_t z = xapi_trace_calltree_full(cs, space2, sizeof(space2));
  printf("reconstituted trace\n%.*s\n", (int)z, space2);
#endif

  succeed;
}
