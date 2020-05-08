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

#include "internal.h"
#include "mempolicy/mempolicy.internal.h"
#include "errtab/KERNEL.errtab.h"
#include "errtab/XLINUX.errtab.h"
#include "xstdlib.h"

// arbitrary max policies
#define MAX_POLICIES 8

// active policy
__thread mempolicy * policy;

// policy stack
__thread mempolicy ** policies;
__thread size_t policiesl;
__thread size_t policiesa;

void mempolicy_teardown()
{
  wfree(policies);
}

API xapi mempolicy_push(mempolicy * plc, int * const restrict mpc)
{
  enter;

  if(plc)
  {
    if(policiesl == policiesa)
    {
      if(policiesl == MAX_POLICIES)
        fail(XLINUX_MPOLICY);

      size_t ns = policiesa ?: 2;
      ns = ns * 2 + ns / 2;
      fatal(xrealloc, &policies, sizeof(**policies), ns, policiesa);
      policiesa = ns;
    }

    policies[policiesl++] = plc;
    policy = policies[policiesl - 1];
    if(mpc)
      (*mpc)++;
  }

  finally : coda;
}

API mempolicy * mempolicy_pop(int * const restrict mpc)
{
  mempolicy * r = 0;
  if(policiesl)
  {
    r = policies[policiesl - 1];

    policiesl -= 1;
    if(mpc)
      (*mpc)--;
  }

  if(policiesl)
    policy = policies[policiesl - 1];
  else
    policy = 0;

  return r;
}

API mempolicy * mempolicy_unwind(int * const restrict mpc)
{
  mempolicy * r = 0;
  if(policiesl)
    r = policies[policiesl - 1];

  policiesl = 0;
  if(mpc)
    (*mpc) = 0;

  policy = 0;

  return r;
}
