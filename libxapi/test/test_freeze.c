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
#include "xapi/trace.h"

/*

SUMMARY
 test the functionality of xapi_calltree_freeze

*/

xapi beta(int num)
{
  enter;

  fail(TEST_ERROR_ONE);

  finally : coda;
}

xapi alpha(int num)
{
  enter;

  fatal(beta, num);

  finally : coda;
}

char space[4096 << 2];
char space2[4096];

xapi foo()
{
  enter;

#if XAPI_STACKTRACE_INCL
  memblk * mb;
#endif

  fatal(alpha, 125);

finally:
#if XAPI_STACKTRACE_INCL
  mb = xapi_calltree_freeze();
  memblk_copyto(mb, space, sizeof(space));
  xapi_calltree_unfreeze();
#endif
coda;
}

int main()
{
#if XAPI_STACKTRACE_INCL
  xapi_errtab_register(perrtab_TEST);
#endif

  xapi exit = foo();
  assert_exit(exit, perrtab_TEST, TEST_ERROR_ONE);

#if XAPI_STACKTRACE_INCL
  struct calltree * ct = xapi_calltree_thaw(space);
  size_t z = xapi_trace_calltree_full(ct, space2, sizeof(space2));
  printf("reconstituted trace\n%.*s\n", (int)z, space2);
#endif

  succeed;
}
